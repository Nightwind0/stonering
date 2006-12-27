#include "SteelParser.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

#define DEBUG_SPEW_1(x) if (m_debug_spew_level >= 1) std::cerr << x
#define DEBUG_SPEW_2(x) if (m_debug_spew_level >= 2) std::cerr << x



SteelParser::SteelParser ()

{

    m_debug_spew_level = 0;
    DEBUG_SPEW_2("### number of state transitions = " << ms_state_transition_count << std::endl);
    m_reduction_token = static_cast<int>(0);
}

SteelParser::~SteelParser ()
{

}

void SteelParser::CheckStateConsistency ()
{
    unsigned int counter = 1;
    for (unsigned int i = 0; i < ms_state_count; ++i)
    {
        if (ms_state[i].m_lookahead_transition_offset > 0)
        {
            assert(counter == ms_state[i].m_lookahead_transition_offset);
            assert(ms_state[i].m_lookahead_transition_count > 0);
        }
        else
            assert(ms_state[i].m_lookahead_transition_count == 0);

        counter += ms_state[i].m_lookahead_transition_count;

        if (ms_state[i].m_default_action_offset > 0)
            ++counter;

        if (ms_state[i].m_non_terminal_transition_offset > 0)
        {
            assert(counter == ms_state[i].m_non_terminal_transition_offset);
            assert(ms_state[i].m_non_terminal_transition_offset > 0);
        }
        else
            assert(ms_state[i].m_non_terminal_transition_offset == 0);

        counter += ms_state[i].m_non_terminal_transition_count;
    }
    assert(counter == ms_state_transition_count);
}

SteelParser::ParserReturnCode SteelParser::Parse ()
{


    ParserReturnCode return_code = PrivateParse();



    return return_code;
}

bool SteelParser::GetDoesStateAcceptErrorToken (SteelParser::StateNumber state_number) const
{
    assert(state_number < ms_state_count);
    State const &state = ms_state[state_number];

    for (unsigned int transition = state.m_lookahead_transition_offset,
                      transition_end = state.m_lookahead_transition_offset +
                                       state.m_lookahead_transition_count;
         transition < transition_end;
         ++transition)
    {
        if (ms_state_transition[transition].m_token_type == Token::ERROR_)
            return true;
    }

    return false;
}

SteelParser::ParserReturnCode SteelParser::PrivateParse ()
{
    m_state_stack.clear();
    m_token_stack.clear();

    m_lookahead_token_type = Token::INVALID_;
    m_lookahead_token = static_cast<int>(0);
    m_is_new_lookahead_token_required = true;

    m_saved_lookahead_token_type = Token::INVALID_;
    m_get_new_lookahead_token_type_from_saved = false;
    m_previous_transition_accepted_error_token = false;

    m_is_returning_with_non_terminal = false;
    m_returning_with_this_non_terminal = Token::INVALID_;

    // start in state 0
    PushState(0);

    while (true)
    {
        StateNumber current_state_number = m_state_stack.back();
        assert(current_state_number < ms_state_count);
        State const &current_state = ms_state[current_state_number];

        unsigned int state_transition_number;
        unsigned int state_transition_count;
        unsigned int default_action_state_transition_number;
        Token::Type state_transition_token_type = Token::INVALID_;

        // if we've just reduced to a non-terminal, coming from
        // another state, use the non-terminal transitions.
        if (m_is_returning_with_non_terminal)
        {
            m_is_returning_with_non_terminal = false;
            state_transition_number = current_state.m_non_terminal_transition_offset;
            state_transition_count = current_state.m_non_terminal_transition_count;
            default_action_state_transition_number = 0;
            state_transition_token_type = m_returning_with_this_non_terminal;
        }
        // otherwise use the lookahead transitions, with the default action
        else
        {
            state_transition_number = current_state.m_lookahead_transition_offset;
            state_transition_count = current_state.m_lookahead_transition_count;
            default_action_state_transition_number = current_state.m_default_action_offset;
            // GetLookaheadTokenType may cause Scan to be called, which may
            // block execution.  only scan a token if necessary.
            if (state_transition_count != 0)
            {
                state_transition_token_type = GetLookaheadTokenType();
                DEBUG_SPEW_1("*** lookahead token type: " << state_transition_token_type << std::endl);
            }
        }

        unsigned int i;
        for (i = 0;
             i < state_transition_count;
             ++i, ++state_transition_number)
        {
            StateTransition const &state_transition =
                ms_state_transition[state_transition_number];
            // if this token matches the current transition, do its action
            if (state_transition.m_token_type == state_transition_token_type)
            {
                if (state_transition.m_token_type == Token::ERROR_)
                    m_previous_transition_accepted_error_token = true;
                else
                    m_previous_transition_accepted_error_token = false;

                PrintStateTransition(state_transition_number);
                if (ProcessAction(state_transition.m_action) == ARC_ACCEPT_AND_RETURN)
                    return PRC_SUCCESS; // the accepted token is in m_reduction_token
                else
                    break;
            }
        }

        // if no transition matched, check for a default action.
        if (i == state_transition_count)
        {
            // check for the default action
            if (default_action_state_transition_number != 0)
            {
                PrintStateTransition(default_action_state_transition_number);
                Action const &default_action =
                    ms_state_transition[default_action_state_transition_number].m_action;
                if (ProcessAction(default_action) == ARC_ACCEPT_AND_RETURN)
                    return PRC_SUCCESS; // the accepted token is in m_reduction_token
            }
            // otherwise go into error recovery mode
            else
            {
                assert(!m_is_new_lookahead_token_required);

                DEBUG_SPEW_1("!!! error recovery: begin" << std::endl);

                // if an error was encountered, and this state accepts the %error
                // token, then we don't need to pop states
                if (GetDoesStateAcceptErrorToken(current_state_number))
                {
                    // if an error token was previously accepted, then throw
                    // away the lookahead token, because whatever the lookahead
                    // was didn't match.  this prevents an infinite loop.
                    if (m_previous_transition_accepted_error_token)
                    {
                        ThrowAwayToken(m_lookahead_token);
                        m_is_new_lookahead_token_required = true;
                    }
                    // otherwise, save off the lookahead token so that once the
                    // %error token has been shifted, the lookahead can be
                    // re-analyzed.
                    else
                    {
                        m_saved_lookahead_token_type = m_lookahead_token_type;
                        m_get_new_lookahead_token_type_from_saved = true;
                        m_lookahead_token_type = Token::ERROR_;
                    }
                }
                // otherwise save off the lookahead token for the error panic popping
                else
                {
                    // save off the lookahead token type and set the current to Token::ERROR_
                    m_saved_lookahead_token_type = m_lookahead_token_type;
                    m_get_new_lookahead_token_type_from_saved = true;
                    m_lookahead_token_type = Token::ERROR_;

                    // pop until we either run off the stack, or find a state
                    // which accepts the %error token.
                    assert(m_state_stack.size() > 0);
                    do
                    {
                        DEBUG_SPEW_1("!!! error recovery: popping state " << current_state_number << std::endl);
                        m_state_stack.pop_back();

                        if (m_state_stack.size() == 0)
                        {
                            ThrowAwayTokenStack();
                            DEBUG_SPEW_1("!!! error recovery: unhandled error -- quitting" << std::endl);
                            return PRC_UNHANDLED_PARSE_ERROR;
                        }

                        assert(m_token_stack.size() > 0);
                        ThrowAwayToken(m_token_stack.back());
                        m_token_stack.pop_back();
                        current_state_number = m_state_stack.back();
                    }
                    while (!GetDoesStateAcceptErrorToken(current_state_number));
                }

                DEBUG_SPEW_1("!!! error recovery: found state which accepts %error token" << std::endl);
                PrintStateStack();
            }
        }
    }

    // this should never happen because the above loop is infinite
    return PRC_UNHANDLED_PARSE_ERROR;
}

SteelParser::ActionReturnCode SteelParser::ProcessAction (SteelParser::Action const &action)
{
    if (action.m_transition_action == TA_SHIFT_AND_PUSH_STATE)
    {
        ShiftLookaheadToken();
        PushState(action.m_data);
    }
    else if (action.m_transition_action == TA_PUSH_STATE)
    {
        PushState(action.m_data);
    }
    else if (action.m_transition_action == TA_REDUCE_USING_RULE)
    {
        unsigned int reduction_rule_number = action.m_data;
        assert(reduction_rule_number < ms_reduction_rule_count);
        ReductionRule const &reduction_rule = ms_reduction_rule[reduction_rule_number];
        ReduceUsingRule(reduction_rule, false);
    }
    else if (action.m_transition_action == TA_REDUCE_AND_ACCEPT_USING_RULE)
    {
        unsigned int reduction_rule_number = action.m_data;
        assert(reduction_rule_number < ms_reduction_rule_count);
        ReductionRule const &reduction_rule = ms_reduction_rule[reduction_rule_number];
        ReduceUsingRule(reduction_rule, true);
        DEBUG_SPEW_1("*** accept" << std::endl);
        // everything is done, so just return.
        return ARC_ACCEPT_AND_RETURN;
    }
    else if (action.m_transition_action == TA_THROW_AWAY_LOOKAHEAD_TOKEN)
    {
        assert(!m_is_new_lookahead_token_required);
        ThrowAwayToken(m_lookahead_token);
        m_is_new_lookahead_token_required = true;
    }

    return ARC_CONTINUE_PARSING;
}

void SteelParser::ShiftLookaheadToken ()
{
    assert(m_lookahead_token_type != Token::DEFAULT_);
    assert(m_lookahead_token_type != Token::INVALID_);
    DEBUG_SPEW_1("*** shifting lookahead token -- type " << m_lookahead_token_type << std::endl);
    m_token_stack.push_back(m_lookahead_token);
    m_is_new_lookahead_token_required = true;
}

void SteelParser::PushState (StateNumber const state_number)
{
    assert(state_number < ms_state_count);

    DEBUG_SPEW_1("*** going to state " << state_number << std::endl);
    m_state_stack.push_back(state_number);
    PrintStateStack();
}

void SteelParser::ReduceUsingRule (ReductionRule const &reduction_rule, bool and_accept)
{
    if (and_accept)
    {
        assert(reduction_rule.m_number_of_tokens_to_reduce_by == m_state_stack.size() - 1);
        assert(reduction_rule.m_number_of_tokens_to_reduce_by == m_token_stack.size());
    }
    else
    {
        assert(reduction_rule.m_number_of_tokens_to_reduce_by < m_state_stack.size());
        assert(reduction_rule.m_number_of_tokens_to_reduce_by <= m_token_stack.size());
    }

    DEBUG_SPEW_1("*** reducing: " << reduction_rule.m_description << std::endl);

    m_is_returning_with_non_terminal = true;
    m_returning_with_this_non_terminal = reduction_rule.m_non_terminal_to_reduce_to;
    m_reduction_rule_token_count = reduction_rule.m_number_of_tokens_to_reduce_by;

    // call the reduction rule handler if it exists
    if (reduction_rule.m_handler != NULL)
        m_reduction_token = (this->*(reduction_rule.m_handler))();
    // pop the states and tokens
    PopStates(reduction_rule.m_number_of_tokens_to_reduce_by, false);

    // only push the reduced token if we aren't accepting yet
    if (!and_accept)
    {
        // push the token that resulted from the reduction
        m_token_stack.push_back(m_reduction_token);
        PrintStateStack();
    }
}

void SteelParser::PopStates (unsigned int number_of_states_to_pop, bool print_state_stack)
{
    assert(number_of_states_to_pop < m_state_stack.size());
    assert(number_of_states_to_pop <= m_token_stack.size());

    while (number_of_states_to_pop-- > 0)
    {
        m_state_stack.pop_back();
        m_token_stack.pop_back();
    }

    if (print_state_stack)
        PrintStateStack();
}

void SteelParser::PrintStateStack () const
{
    DEBUG_SPEW_2("*** state stack: ");
    for (StateStack::const_iterator it = m_state_stack.begin(),
                                 it_end = m_state_stack.end();
         it != it_end;
         ++it)
    {
        DEBUG_SPEW_2(*it << " ");
    }
    DEBUG_SPEW_2(std::endl);
}

void SteelParser::PrintStateTransition (unsigned int const state_transition_number) const
{
    assert(state_transition_number < ms_state_transition_count);
    DEBUG_SPEW_2("&&& exercising state transition " << std::setw(4) << std::right << state_transition_number << std::endl);
}

void SteelParser::ScanANewLookaheadToken ()
{
    assert(!m_is_new_lookahead_token_required);
    m_lookahead_token = static_cast<int>(0);
    m_lookahead_token_type = Scan();
    DEBUG_SPEW_1("*** scanned a new lookahead token -- type " << m_lookahead_token_type << std::endl);
}

void SteelParser::ThrowAwayToken (int token)
{

}

void SteelParser::ThrowAwayTokenStack ()
{
    while (!m_token_stack.empty())
    {
        ThrowAwayToken(m_token_stack.back());
        m_token_stack.pop_back();
    }
}

std::ostream &operator << (std::ostream &stream, SteelParser::Token::Type token_type)
{
    static std::string const s_token_type_string[] =
    {
        "AND",
        "ARRAY_IDENTIFIER",
        "BREAK",
        "CONTINUE",
        "D",
        "DECREMENT",
        "ELSE",
        "EQ",
        "FLOAT",
        "FUNCTION",
        "FUNC_IDENTIFIER",
        "GT",
        "GTE",
        "IF",
        "INCREMENT",
        "INT",
        "LOOP",
        "LT",
        "LTE",
        "NE",
        "NOT",
        "OR",
        "RETURN",
        "STRING",
        "TIMES",
        "USING",
        "VAR",
        "VAR_IDENTIFIER",
        "WHILE",
        "END_",

        "call",
        "exp",
        "func_definition",
        "func_definition_list",
        "param_definition",
        "param_list",
        "root",
        "statement",
        "statement_list",
        "vardecl",
        "START_",

        "%error",
        "DEFAULT_",
        "INVALID_"
    };
    static unsigned int const s_token_type_string_count =
        sizeof(s_token_type_string) /
        sizeof(std::string);

    unsigned token_type_value = static_cast<unsigned int>(token_type);
    if (token_type_value < 0x20)
        stream << token_type_value;
    else if (token_type_value < 0x7F)
        stream << "'" << static_cast<char>(token_type) << "'";
    else if (token_type_value < 0x100)
        stream << token_type_value;
    else if (token_type_value < 0x100 + s_token_type_string_count)
        stream << s_token_type_string[token_type_value - 0x100];
    else
        stream << token_type_value;

    return stream;
}

// ///////////////////////////////////////////////////////////////////////////
// state machine reduction rule handlers
// ///////////////////////////////////////////////////////////////////////////

// rule 0: %start <- root END_    
int SteelParser::ReductionRuleHandler0000 ()
{
    assert(0 < m_reduction_rule_token_count);
    return m_token_stack[m_token_stack.size() - m_reduction_rule_token_count];

    return static_cast<int>(0);
}

// rule 1: root <- statement_list    
int SteelParser::ReductionRuleHandler0001 ()
{
    return static_cast<int>(0);
}

// rule 2: root <- func_definition_list    
int SteelParser::ReductionRuleHandler0002 ()
{
    return static_cast<int>(0);
}

// rule 3: func_definition_list <- func_definition    
int SteelParser::ReductionRuleHandler0003 ()
{
    return static_cast<int>(0);
}

// rule 4: func_definition_list <- func_definition_list func_definition    
int SteelParser::ReductionRuleHandler0004 ()
{
    return static_cast<int>(0);
}

// rule 5: func_definition <- FUNCTION FUNC_IDENTIFIER '(' param_definition ')' statement    
int SteelParser::ReductionRuleHandler0005 ()
{
    return static_cast<int>(0);
}

// rule 6: param_definition <- VAR_IDENTIFIER    
int SteelParser::ReductionRuleHandler0006 ()
{
    return static_cast<int>(0);
}

// rule 7: param_definition <- param_definition ',' VAR_IDENTIFIER    
int SteelParser::ReductionRuleHandler0007 ()
{
    return static_cast<int>(0);
}

// rule 8: statement_list <-     
int SteelParser::ReductionRuleHandler0008 ()
{
    return static_cast<int>(0);
}

// rule 9: statement_list <- statement_list statement    
int SteelParser::ReductionRuleHandler0009 ()
{
    return static_cast<int>(0);
}

// rule 10: statement <- exp ';'    
int SteelParser::ReductionRuleHandler0010 ()
{
    return static_cast<int>(0);
}

// rule 11: statement <- '{' statement_list '}'    
int SteelParser::ReductionRuleHandler0011 ()
{
    return static_cast<int>(0);
}

// rule 12: statement <- WHILE '(' exp ')' statement    
int SteelParser::ReductionRuleHandler0012 ()
{
    return static_cast<int>(0);
}

// rule 13: statement <- IF '(' exp ')' statement ELSE statement     %prec ELSE
int SteelParser::ReductionRuleHandler0013 ()
{
    return static_cast<int>(0);
}

// rule 14: statement <- IF '(' exp ')' statement     %prec NON_ELSE
int SteelParser::ReductionRuleHandler0014 ()
{
    return static_cast<int>(0);
}

// rule 15: statement <- RETURN exp ';'    
int SteelParser::ReductionRuleHandler0015 ()
{
    return static_cast<int>(0);
}

// rule 16: statement <- LOOP '(' exp ')' TIMES USING VAR_IDENTIFIER statement    
int SteelParser::ReductionRuleHandler0016 ()
{
    return static_cast<int>(0);
}

// rule 17: exp <- call    
int SteelParser::ReductionRuleHandler0017 ()
{
    return static_cast<int>(0);
}

// rule 18: exp <- vardecl    
int SteelParser::ReductionRuleHandler0018 ()
{
    return static_cast<int>(0);
}

// rule 19: exp <- INT    
int SteelParser::ReductionRuleHandler0019 ()
{
    return static_cast<int>(0);
}

// rule 20: exp <- FLOAT    
int SteelParser::ReductionRuleHandler0020 ()
{
    return static_cast<int>(0);
}

// rule 21: exp <- VAR_IDENTIFIER    
int SteelParser::ReductionRuleHandler0021 ()
{
    return static_cast<int>(0);
}

// rule 22: exp <- exp '+' exp    %left %prec ADD_SUB
int SteelParser::ReductionRuleHandler0022 ()
{
    return static_cast<int>(0);
}

// rule 23: exp <- exp '-' exp    %left %prec ADD_SUB
int SteelParser::ReductionRuleHandler0023 ()
{
    return static_cast<int>(0);
}

// rule 24: exp <- exp '*' exp    %left %prec MULT_DIV_MOD
int SteelParser::ReductionRuleHandler0024 ()
{
    return static_cast<int>(0);
}

// rule 25: exp <- exp '/' exp    %left %prec MULT_DIV_MOD
int SteelParser::ReductionRuleHandler0025 ()
{
    return static_cast<int>(0);
}

// rule 26: exp <- exp '%' exp    %left %prec MULT_DIV_MOD
int SteelParser::ReductionRuleHandler0026 ()
{
    return static_cast<int>(0);
}

// rule 27: exp <- exp D exp    %left %prec MULT_DIV_MOD
int SteelParser::ReductionRuleHandler0027 ()
{
    return static_cast<int>(0);
}

// rule 28: exp <- exp '^' exp    %right %prec POW
int SteelParser::ReductionRuleHandler0028 ()
{
    return static_cast<int>(0);
}

// rule 29: exp <- exp OR exp    %left %prec OR
int SteelParser::ReductionRuleHandler0029 ()
{
    return static_cast<int>(0);
}

// rule 30: exp <- exp AND exp    %left %prec AND
int SteelParser::ReductionRuleHandler0030 ()
{
    return static_cast<int>(0);
}

// rule 31: exp <- exp EQ exp    %left %prec EQ_NE
int SteelParser::ReductionRuleHandler0031 ()
{
    return static_cast<int>(0);
}

// rule 32: exp <- exp NE exp    %left %prec EQ_NE
int SteelParser::ReductionRuleHandler0032 ()
{
    return static_cast<int>(0);
}

// rule 33: exp <- exp LT exp    %left %prec COMPARATOR
int SteelParser::ReductionRuleHandler0033 ()
{
    return static_cast<int>(0);
}

// rule 34: exp <- exp GT exp    %left %prec COMPARATOR
int SteelParser::ReductionRuleHandler0034 ()
{
    return static_cast<int>(0);
}

// rule 35: exp <- exp LTE exp    %left %prec COMPARATOR
int SteelParser::ReductionRuleHandler0035 ()
{
    return static_cast<int>(0);
}

// rule 36: exp <- exp GTE exp    %left %prec COMPARATOR
int SteelParser::ReductionRuleHandler0036 ()
{
    return static_cast<int>(0);
}

// rule 37: exp <- '(' exp ')'     %prec PAREN
int SteelParser::ReductionRuleHandler0037 ()
{
    return static_cast<int>(0);
}

// rule 38: exp <- '-' exp     %prec UNARY
int SteelParser::ReductionRuleHandler0038 ()
{
    return static_cast<int>(0);
}

// rule 39: exp <- NOT exp     %prec UNARY
int SteelParser::ReductionRuleHandler0039 ()
{
    return static_cast<int>(0);
}

// rule 40: exp <- INCREMENT exp     %prec UNARY
int SteelParser::ReductionRuleHandler0040 ()
{
    return static_cast<int>(0);
}

// rule 41: exp <- DECREMENT exp     %prec UNARY
int SteelParser::ReductionRuleHandler0041 ()
{
    return static_cast<int>(0);
}

// rule 42: exp <- exp INCREMENT     %prec PAREN
int SteelParser::ReductionRuleHandler0042 ()
{
    return static_cast<int>(0);
}

// rule 43: exp <- exp DECREMENT     %prec PAREN
int SteelParser::ReductionRuleHandler0043 ()
{
    return static_cast<int>(0);
}

// rule 44: exp <- ARRAY_IDENTIFIER '[' exp ']'     %prec PAREN
int SteelParser::ReductionRuleHandler0044 ()
{
    return static_cast<int>(0);
}

// rule 45: call <- FUNC_IDENTIFIER '(' param_list ')'    
int SteelParser::ReductionRuleHandler0045 ()
{
    return static_cast<int>(0);
}

// rule 46: vardecl <- VAR VAR_IDENTIFIER    
int SteelParser::ReductionRuleHandler0046 ()
{
    return static_cast<int>(0);
}

// rule 47: vardecl <- VAR VAR_IDENTIFIER '=' exp    
int SteelParser::ReductionRuleHandler0047 ()
{
    return static_cast<int>(0);
}

// rule 48: vardecl <- VAR ARRAY_IDENTIFIER '[' INT ']'    
int SteelParser::ReductionRuleHandler0048 ()
{
    return static_cast<int>(0);
}

// rule 49: param_list <- exp    
int SteelParser::ReductionRuleHandler0049 ()
{
    return static_cast<int>(0);
}

// rule 50: param_list <- param_list ',' exp    
int SteelParser::ReductionRuleHandler0050 ()
{
    return static_cast<int>(0);
}



// ///////////////////////////////////////////////////////////////////////////
// reduction rule lookup table
// ///////////////////////////////////////////////////////////////////////////

SteelParser::ReductionRule const SteelParser::ms_reduction_rule[] =
{
    {                 Token::START_,  2, &SteelParser::ReductionRuleHandler0000, "rule 0: %start <- root END_    "},
    {                 Token::root__,  1, &SteelParser::ReductionRuleHandler0001, "rule 1: root <- statement_list    "},
    {                 Token::root__,  1, &SteelParser::ReductionRuleHandler0002, "rule 2: root <- func_definition_list    "},
    { Token::func_definition_list__,  1, &SteelParser::ReductionRuleHandler0003, "rule 3: func_definition_list <- func_definition    "},
    { Token::func_definition_list__,  2, &SteelParser::ReductionRuleHandler0004, "rule 4: func_definition_list <- func_definition_list func_definition    "},
    {      Token::func_definition__,  6, &SteelParser::ReductionRuleHandler0005, "rule 5: func_definition <- FUNCTION FUNC_IDENTIFIER '(' param_definition ')' statement    "},
    {     Token::param_definition__,  1, &SteelParser::ReductionRuleHandler0006, "rule 6: param_definition <- VAR_IDENTIFIER    "},
    {     Token::param_definition__,  3, &SteelParser::ReductionRuleHandler0007, "rule 7: param_definition <- param_definition ',' VAR_IDENTIFIER    "},
    {       Token::statement_list__,  0, &SteelParser::ReductionRuleHandler0008, "rule 8: statement_list <-     "},
    {       Token::statement_list__,  2, &SteelParser::ReductionRuleHandler0009, "rule 9: statement_list <- statement_list statement    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0010, "rule 10: statement <- exp ';'    "},
    {            Token::statement__,  3, &SteelParser::ReductionRuleHandler0011, "rule 11: statement <- '{' statement_list '}'    "},
    {            Token::statement__,  5, &SteelParser::ReductionRuleHandler0012, "rule 12: statement <- WHILE '(' exp ')' statement    "},
    {            Token::statement__,  7, &SteelParser::ReductionRuleHandler0013, "rule 13: statement <- IF '(' exp ')' statement ELSE statement     %prec ELSE"},
    {            Token::statement__,  5, &SteelParser::ReductionRuleHandler0014, "rule 14: statement <- IF '(' exp ')' statement     %prec NON_ELSE"},
    {            Token::statement__,  3, &SteelParser::ReductionRuleHandler0015, "rule 15: statement <- RETURN exp ';'    "},
    {            Token::statement__,  8, &SteelParser::ReductionRuleHandler0016, "rule 16: statement <- LOOP '(' exp ')' TIMES USING VAR_IDENTIFIER statement    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0017, "rule 17: exp <- call    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0018, "rule 18: exp <- vardecl    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0019, "rule 19: exp <- INT    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0020, "rule 20: exp <- FLOAT    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0021, "rule 21: exp <- VAR_IDENTIFIER    "},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0022, "rule 22: exp <- exp '+' exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0023, "rule 23: exp <- exp '-' exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0024, "rule 24: exp <- exp '*' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0025, "rule 25: exp <- exp '/' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0026, "rule 26: exp <- exp '%' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0027, "rule 27: exp <- exp D exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0028, "rule 28: exp <- exp '^' exp    %right %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0029, "rule 29: exp <- exp OR exp    %left %prec OR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0030, "rule 30: exp <- exp AND exp    %left %prec AND"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0031, "rule 31: exp <- exp EQ exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0032, "rule 32: exp <- exp NE exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0033, "rule 33: exp <- exp LT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0034, "rule 34: exp <- exp GT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0035, "rule 35: exp <- exp LTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0036, "rule 36: exp <- exp GTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0037, "rule 37: exp <- '(' exp ')'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0038, "rule 38: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0039, "rule 39: exp <- NOT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0040, "rule 40: exp <- INCREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0041, "rule 41: exp <- DECREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0042, "rule 42: exp <- exp INCREMENT     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0043, "rule 43: exp <- exp DECREMENT     %prec PAREN"},
    {                  Token::exp__,  4, &SteelParser::ReductionRuleHandler0044, "rule 44: exp <- ARRAY_IDENTIFIER '[' exp ']'     %prec PAREN"},
    {                 Token::call__,  4, &SteelParser::ReductionRuleHandler0045, "rule 45: call <- FUNC_IDENTIFIER '(' param_list ')'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0046, "rule 46: vardecl <- VAR VAR_IDENTIFIER    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0047, "rule 47: vardecl <- VAR VAR_IDENTIFIER '=' exp    "},
    {              Token::vardecl__,  5, &SteelParser::ReductionRuleHandler0048, "rule 48: vardecl <- VAR ARRAY_IDENTIFIER '[' INT ']'    "},
    {           Token::param_list__,  1, &SteelParser::ReductionRuleHandler0049, "rule 49: param_list <- exp    "},
    {           Token::param_list__,  3, &SteelParser::ReductionRuleHandler0050, "rule 50: param_list <- param_list ',' exp    "},

    // special error panic reduction rule
    {                 Token::ERROR_,  1,                                     NULL, "* -> error"}
};

unsigned int const SteelParser::ms_reduction_rule_count =
    sizeof(SteelParser::ms_reduction_rule) /
    sizeof(SteelParser::ReductionRule);

// ///////////////////////////////////////////////////////////////////////////
// state transition lookup table
// ///////////////////////////////////////////////////////////////////////////

SteelParser::State const SteelParser::ms_state[] =
{
    {   1,    1,    2,    3,    4}, // state    0
    {   7,    1,    0,    0,    0}, // state    1
    {   8,    1,    0,    0,    0}, // state    2
    {   9,    1,   10,   11,    1}, // state    3
    {   0,    0,   12,    0,    0}, // state    4
    {  13,   16,   29,   30,    4}, // state    5
    {  34,    1,    0,    0,    0}, // state    6
    {   0,    0,   35,    0,    0}, // state    7
    {   0,    0,   36,    0,    0}, // state    8
    {  37,   11,    0,   48,    3}, // state    9
    {   0,    0,   51,   52,    1}, // state   10
    {  53,   11,    0,   64,    3}, // state   11
    {  67,   11,    0,   78,    3}, // state   12
    {  81,    1,    0,    0,    0}, // state   13
    {  82,   11,    0,   93,    3}, // state   14
    {  96,    1,    0,    0,    0}, // state   15
    {  97,    1,    0,    0,    0}, // state   16
    {   0,    0,   98,    0,    0}, // state   17
    {  99,    1,    0,    0,    0}, // state   18
    { 100,    1,    0,    0,    0}, // state   19
    { 101,    2,    0,    0,    0}, // state   20
    {   0,    0,  103,    0,    0}, // state   21
    {   0,    0,  104,    0,    0}, // state   22
    { 105,   11,    0,  116,    3}, // state   23
    { 119,   11,    0,  130,    3}, // state   24
    {   0,    0,  133,    0,    0}, // state   25
    { 134,   18,    0,    0,    0}, // state   26
    {   0,    0,  152,    0,    0}, // state   27
    {   0,    0,  153,    0,    0}, // state   28
    { 154,    1,    0,  155,    1}, // state   29
    { 156,   18,    0,    0,    0}, // state   30
    { 174,   17,    0,  191,    4}, // state   31
    { 195,    2,  197,    0,    0}, // state   32
    { 198,    2,  200,    0,    0}, // state   33
    { 201,   11,    0,  212,    3}, // state   34
    { 215,   18,    0,    0,    0}, // state   35
    { 233,   11,    0,  244,    3}, // state   36
    { 247,   11,    0,  258,    4}, // state   37
    { 262,   11,    0,  273,    3}, // state   38
    { 276,   11,    0,  287,    3}, // state   39
    { 290,    1,  291,    0,    0}, // state   40
    { 292,    1,    0,    0,    0}, // state   41
    { 293,    2,  295,    0,    0}, // state   42
    { 296,    2,  298,    0,    0}, // state   43
    {   0,    0,  299,    0,    0}, // state   44
    { 300,   11,    0,  311,    3}, // state   45
    { 314,   11,    0,  325,    3}, // state   46
    { 328,   11,    0,  339,    3}, // state   47
    { 342,   11,    0,  353,    3}, // state   48
    { 356,   11,    0,  367,    3}, // state   49
    { 370,   11,    0,  381,    3}, // state   50
    { 384,   11,    0,  395,    3}, // state   51
    { 398,   11,    0,  409,    3}, // state   52
    { 412,   11,    0,  423,    3}, // state   53
    { 426,   11,    0,  437,    3}, // state   54
    { 440,   11,    0,  451,    3}, // state   55
    { 454,   11,    0,  465,    3}, // state   56
    { 468,   11,    0,  479,    3}, // state   57
    { 482,   11,    0,  493,    3}, // state   58
    { 496,   11,    0,  507,    3}, // state   59
    {   0,    0,  510,    0,    0}, // state   60
    {   0,    0,  511,    0,    0}, // state   61
    {   0,    0,  512,    0,    0}, // state   62
    { 513,    2,    0,    0,    0}, // state   63
    {   0,    0,  515,    0,    0}, // state   64
    {   0,    0,  516,    0,    0}, // state   65
    { 517,   18,    0,    0,    0}, // state   66
    {   0,    0,  535,    0,    0}, // state   67
    { 536,   18,    0,    0,    0}, // state   68
    { 554,   17,  571,    0,    0}, // state   69
    { 572,    2,    0,    0,    0}, // state   70
    { 574,   18,    0,    0,    0}, // state   71
    { 592,   18,    0,    0,    0}, // state   72
    { 610,   11,    0,  621,    3}, // state   73
    { 624,    1,    0,    0,    0}, // state   74
    { 625,    7,  632,    0,    0}, // state   75
    { 633,    7,  640,    0,    0}, // state   76
    { 641,    3,  644,    0,    0}, // state   77
    { 645,    3,  648,    0,    0}, // state   78
    { 649,    3,  652,    0,    0}, // state   79
    { 653,    3,  656,    0,    0}, // state   80
    { 657,    3,  660,    0,    0}, // state   81
    { 661,    9,  670,    0,    0}, // state   82
    { 671,    9,  680,    0,    0}, // state   83
    { 681,   13,  694,    0,    0}, // state   84
    { 695,   13,  708,    0,    0}, // state   85
    { 709,    9,  718,    0,    0}, // state   86
    { 719,    9,  728,    0,    0}, // state   87
    { 729,   15,  744,    0,    0}, // state   88
    { 745,   16,  761,    0,    0}, // state   89
    { 762,   16,    0,  778,    4}, // state   90
    { 782,    1,    0,    0,    0}, // state   91
    { 783,   16,    0,  799,    4}, // state   92
    { 803,   16,    0,  819,    4}, // state   93
    {   0,    0,  823,    0,    0}, // state   94
    { 824,   11,    0,  835,    3}, // state   95
    {   0,    0,  838,    0,    0}, // state   96
    { 839,    1,    0,    0,    0}, // state   97
    { 840,   17,  857,    0,    0}, // state   98
    { 858,    1,    0,    0,    0}, // state   99
    {   0,    0,  859,    0,    0}, // state  100
    {   0,    0,  860,    0,    0}, // state  101
    {   0,    0,  861,    0,    0}, // state  102
    { 862,    1,  863,    0,    0}, // state  103
    { 864,   17,  881,    0,    0}, // state  104
    { 882,    1,    0,    0,    0}, // state  105
    {   0,    0,  883,    0,    0}, // state  106
    { 884,   16,    0,  900,    4}, // state  107
    { 904,    1,    0,    0,    0}, // state  108
    {   0,    0,  905,    0,    0}, // state  109
    { 906,   16,    0,  922,    4}, // state  110
    {   0,    0,  926,    0,    0}  // state  111

};

unsigned int const SteelParser::ms_state_count =
    sizeof(SteelParser::ms_state) /
    sizeof(SteelParser::State);

// ///////////////////////////////////////////////////////////////////////////
// state transition table
// ///////////////////////////////////////////////////////////////////////////

SteelParser::StateTransition const SteelParser::ms_state_transition[] =
{
    // dummy transition in the NULL transition
    {               Token::INVALID_, {                       TA_COUNT,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    0
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},
    // nonterminal transitions
    {                 Token::root__, {                  TA_PUSH_STATE,    2}},
    { Token::func_definition_list__, {                  TA_PUSH_STATE,    3}},
    {      Token::func_definition__, {                  TA_PUSH_STATE,    4}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state    2
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,    1}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    2}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state    4
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state    5
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,   25}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state    6
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    4}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},
    // nonterminal transitions
    {       Token::statement_list__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   32}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   35}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   18
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state   19
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state   20
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   40}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state   21
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   22
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   42}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   43}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   44}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,   63}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,   25}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   66}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   68}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   69}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},
    {           Token::param_list__, {                  TA_PUSH_STATE,   70}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   71}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   72}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   40
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state   41
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   74}},

// ///////////////////////////////////////////////////////////////////////////
// state   42
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state   43
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state   44
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   45
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   75}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   46
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   76}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   47
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   77}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   48
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   78}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   49
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   79}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   50
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   80}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   51
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   81}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   52
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   82}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   53
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   83}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   54
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   84}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   55
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   85}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   56
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   86}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   57
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   87}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   58
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   88}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   59
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   89}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   60
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state   61
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state   62
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state   63
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   90}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,   91}},

// ///////////////////////////////////////////////////////////////////////////
// state   64
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state   65
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   66
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   92}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   67
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state   68
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   93}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   69
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   70
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   94}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,   95}},

// ///////////////////////////////////////////////////////////////////////////
// state   71
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,   96}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   72
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   97}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   73
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   98}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   74
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   99}},

// ///////////////////////////////////////////////////////////////////////////
// state   75
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state   76
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state   77
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   78
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   25}},

// ///////////////////////////////////////////////////////////////////////////
// state   79
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   80
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   26}},

// ///////////////////////////////////////////////////////////////////////////
// state   81
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   27}},

// ///////////////////////////////////////////////////////////////////////////
// state   82
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   83
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   84
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   85
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   86
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   87
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   88
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   30}},

// ///////////////////////////////////////////////////////////////////////////
// state   89
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state   90
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  100}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   91
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,  101}},

// ///////////////////////////////////////////////////////////////////////////
// state   92
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  102}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   93
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  103}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   94
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state   95
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  104}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   96
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state   97
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                  Token::TIMES, {        TA_SHIFT_AND_PUSH_STATE,  105}},

// ///////////////////////////////////////////////////////////////////////////
// state   98
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   99
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  106}},

// ///////////////////////////////////////////////////////////////////////////
// state  100
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state  101
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state  102
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state  103
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::ELSE, {        TA_SHIFT_AND_PUSH_STATE,  107}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state  104
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   47}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   48}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   49}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   50}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state  105
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                  Token::USING, {        TA_SHIFT_AND_PUSH_STATE,  108}},

// ///////////////////////////////////////////////////////////////////////////
// state  106
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state  107
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  109}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state  108
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,  110}},

// ///////////////////////////////////////////////////////////////////////////
// state  109
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state  110
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                   Token::LOOP, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  111}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {                 Token::call__, {                  TA_PUSH_STATE,   27}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state  111
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}}

};

unsigned int const SteelParser::ms_state_transition_count =
    sizeof(SteelParser::ms_state_transition) /
    sizeof(SteelParser::StateTransition);



