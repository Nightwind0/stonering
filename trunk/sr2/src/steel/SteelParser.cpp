#include "SteelParser.h"

#include <cassert>
#include <cstdio>
#include <iomanip>
#include <iostream>

#define DEBUG_SPEW_1(x) if (m_debug_spew_level >= 1) std::cerr << x
#define DEBUG_SPEW_2(x) if (m_debug_spew_level >= 2) std::cerr << x


#line 21 "steel.trison"

	#include "SteelScanner.h"
	#include "Ast.h"

#line 18 "SteelParser.cpp"

SteelParser::SteelParser ()

{

#line 42 "steel.trison"

    m_scanner = new SteelScanner();

#line 28 "SteelParser.cpp"
    m_debug_spew_level = 0;
    DEBUG_SPEW_2("### number of state transitions = " << ms_state_transition_count << std::endl);
    m_reduction_token = static_cast<AstBase*>(0);
}

SteelParser::~SteelParser ()
{

#line 46 "steel.trison"

    delete m_scanner;

#line 41 "SteelParser.cpp"
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
    m_lookahead_token = static_cast<AstBase*>(0);
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
    m_lookahead_token = static_cast<AstBase*>(0);
    m_lookahead_token_type = Scan();
    DEBUG_SPEW_1("*** scanned a new lookahead token -- type " << m_lookahead_token_type << std::endl);
}

void SteelParser::ThrowAwayToken (AstBase* token)
{

#line 51 "steel.trison"

    delete token;

#line 403 "SteelParser.cpp"
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
        "CAT",
        "CONTINUE",
        "D",
        "DECREMENT",
        "ELSE",
        "EQ",
        "FINAL",
        "FLOAT",
        "FOR",
        "FUNCTION",
        "FUNC_IDENTIFIER",
        "GT",
        "GTE",
        "IF",
        "INCREMENT",
        "INT",
        "LT",
        "LTE",
        "NE",
        "NOT",
        "OR",
        "POP",
        "RETURN",
        "STRING",
        "VAR",
        "VAR_IDENTIFIER",
        "WHILE",
        "END_",

        "array_identifier",
        "call",
        "exp",
        "exp_statement",
        "func_definition",
        "func_identifier",
        "int_literal",
        "param_definition",
        "param_id",
        "param_list",
        "root",
        "statement",
        "statement_list",
        "var_identifier",
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
AstBase* SteelParser::ReductionRuleHandler0000 ()
{
    assert(0 < m_reduction_rule_token_count);
    return m_token_stack[m_token_stack.size() - m_reduction_rule_token_count];

    return static_cast<AstBase*>(0);
}

// rule 1: root <- statement_list:list    
AstBase* SteelParser::ReductionRuleHandler0001 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 103 "steel.trison"

				AstScript * pScript =   new AstScript(
							list->GetLine(),
							list->GetScript());
			        pScript->SetList(list);
				return pScript;
			
#line 518 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 2: func_definition <- FUNCTION FUNC_IDENTIFIER:id '(' param_definition:params ')' '{':b1 statement_list:stmts '}':b2    
AstBase* SteelParser::ReductionRuleHandler0002 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstParamDefinitionList* params = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);
    assert(5 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 5]);
    assert(6 < m_reduction_rule_token_count);
    AstStatementList* stmts = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);
    assert(7 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 7]);

#line 116 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									params,
									stmts,
									false);
				
#line 547 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 3: func_definition <- FUNCTION FUNC_IDENTIFIER:id '(' ')' '{':b1 statement_list:stmts '}':b2    
AstBase* SteelParser::ReductionRuleHandler0003 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(4 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);
    assert(5 < m_reduction_rule_token_count);
    AstStatementList* stmts = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 5]);
    assert(6 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);

#line 128 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									NULL,
									stmts,
									false);
				
#line 574 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 4: func_definition <- FINAL FUNCTION FUNC_IDENTIFIER:id '(' param_definition:params ')' '{':b1 statement_list:stmts '}':b2    
AstBase* SteelParser::ReductionRuleHandler0004 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstParamDefinitionList* params = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);
    assert(6 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);
    assert(7 < m_reduction_rule_token_count);
    AstStatementList* stmts = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 7]);
    assert(8 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 8]);

#line 140 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									params,
									stmts,
									true);
				
#line 603 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 5: func_definition <- FINAL FUNCTION FUNC_IDENTIFIER:id '(' ')' '{':b1 statement_list:stmts '}':b2    
AstBase* SteelParser::ReductionRuleHandler0005 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(5 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 5]);
    assert(6 < m_reduction_rule_token_count);
    AstStatementList* stmts = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);
    assert(7 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 7]);

#line 152 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									NULL,
									stmts,
									true);
				
#line 630 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 6: param_id <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0006 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 168 "steel.trison"
 return id; 
#line 642 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 7: param_id <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0007 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 170 "steel.trison"
 return id; 
#line 654 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 8: param_definition <- vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0008 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 176 "steel.trison"

				AstParamDefinitionList * pList = new AstParamDefinitionList(decl->GetLine(),decl->GetScript());
				pList->add(static_cast<AstDeclaration*>(decl));
				return pList;		
			
#line 670 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 9: param_definition <- param_definition:list ',' vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0009 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamDefinitionList* list = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 183 "steel.trison"

				list->add(static_cast<AstDeclaration*>(decl));
				return list;
			
#line 687 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 10: statement_list <- statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0010 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 191 "steel.trison"

				AstStatementList *pList = new AstStatementList( stmt->GetLine(),
										stmt->GetScript());
				pList->add(stmt);
				return pList;
			
#line 704 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 11: statement_list <- statement_list:list statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0011 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 199 "steel.trison"

					list->add( stmt ); 
					return list;
				
#line 721 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 12: statement <- exp_statement:exp    
AstBase* SteelParser::ReductionRuleHandler0012 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 207 "steel.trison"
 return new AstExpressionStatement(exp->GetLine(),exp->GetScript(), exp); 
#line 733 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 13: statement <- func_definition:func    
AstBase* SteelParser::ReductionRuleHandler0013 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinition* func = static_cast< AstFunctionDefinition* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 209 "steel.trison"
 return func; 
#line 745 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 14: statement <- '{':b1 statement_list:list '}':b2    
AstBase* SteelParser::ReductionRuleHandler0014 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 211 "steel.trison"
 delete b1; delete b2; return list; 
#line 761 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 15: statement <- '{':b1 '}':b2    
AstBase* SteelParser::ReductionRuleHandler0015 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 213 "steel.trison"

			 int line = b1->GetLine();
			 std::string script = b1->GetScript();
			 delete b1;
			 delete b2;
			 return new AstStatement(line,script);
			
#line 781 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 16: statement <- vardecl:vardecl ';':semi    
AstBase* SteelParser::ReductionRuleHandler0016 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* vardecl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 221 "steel.trison"
 delete semi; return vardecl; 
#line 795 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 17: statement <- WHILE '(' exp:exp ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0017 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 223 "steel.trison"
 return new AstWhileStatement(exp->GetLine(), exp->GetScript(),exp,stmt); 
#line 809 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 18: statement <- IF '(' exp:exp ')' statement:stmt ELSE statement:elses     %prec ELSE
AstBase* SteelParser::ReductionRuleHandler0018 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);
    assert(6 < m_reduction_rule_token_count);
    AstStatement* elses = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);

#line 225 "steel.trison"
 return new AstIfStatement(exp->GetLine(), exp->GetScript(),exp,stmt,elses);
#line 825 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 19: statement <- IF '(' exp:exp ')' statement:stmt     %prec NON_ELSE
AstBase* SteelParser::ReductionRuleHandler0019 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 227 "steel.trison"
 return new AstIfStatement(exp->GetLine(),exp->GetScript(),exp,stmt); 
#line 839 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 20: statement <- RETURN exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0020 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 229 "steel.trison"
 delete semi; return new AstReturnStatement(exp->GetLine(),exp->GetScript(),exp);
#line 853 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 21: statement <- RETURN ';':semi    
AstBase* SteelParser::ReductionRuleHandler0021 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 232 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstReturnStatement(line,script);
			
#line 870 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 22: statement <- FOR '(' exp_statement:start exp_statement:condition ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0022 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* start = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* condition = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);
    assert(5 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 5]);

#line 240 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition, 
						new AstExpression (start->GetLine(),start->GetScript()), stmt); 
			
#line 889 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 23: statement <- FOR '(' exp_statement:start exp_statement:condition exp:iteration ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0023 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* start = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* condition = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);
    assert(4 < m_reduction_rule_token_count);
    AstExpression* iteration = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);
    assert(6 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);

#line 246 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition,iteration,stmt);
			
#line 909 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 24: statement <- BREAK ';':semi    
AstBase* SteelParser::ReductionRuleHandler0024 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 251 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstBreakStatement(line,script); 
			
#line 926 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 25: statement <- CONTINUE ';':semi    
AstBase* SteelParser::ReductionRuleHandler0025 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 259 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstContinueStatement(line,script); 
			
#line 943 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 26: exp <- call:call    
AstBase* SteelParser::ReductionRuleHandler0026 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstCallExpression* call = static_cast< AstCallExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 270 "steel.trison"
 return call; 
#line 955 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 27: exp <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0027 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 272 "steel.trison"
 return i;
#line 967 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 28: exp <- FLOAT:f    
AstBase* SteelParser::ReductionRuleHandler0028 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* f = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 274 "steel.trison"
 return f; 
#line 979 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 29: exp <- STRING:s    
AstBase* SteelParser::ReductionRuleHandler0029 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* s = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 276 "steel.trison"
 return s; 
#line 991 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 30: exp <- var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0030 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 278 "steel.trison"
 return id; 
#line 1003 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 31: exp <- array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0031 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 280 "steel.trison"
 return id; 
#line 1015 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 32: exp <- exp:a '+' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0032 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 282 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::ADD,a,b); 
#line 1029 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 33: exp <- exp:a '-' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0033 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 284 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::SUB,a,b); 
#line 1043 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 34: exp <- exp:a '*' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0034 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 286 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MULT,a,b); 
#line 1057 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 35: exp <- exp:a '/' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0035 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 288 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::DIV,a,b); 
#line 1071 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 36: exp <- exp:a '%' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0036 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 290 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MOD,a,b); 
#line 1085 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 37: exp <- exp:a D exp:b    %left %prec POW
AstBase* SteelParser::ReductionRuleHandler0037 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 292 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::D,a,b); 
#line 1099 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 38: exp <- exp:lvalue '=' exp:exp    %right %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0038 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 294 "steel.trison"
 return new AstVarAssignmentExpression(lvalue->GetLine(),lvalue->GetScript(),lvalue,exp); 
#line 1113 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 39: exp <- exp:a '^' exp:b    %right %prec POW
AstBase* SteelParser::ReductionRuleHandler0039 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 296 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::POW,a,b); 
#line 1127 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 40: exp <- exp:a OR exp:b    %left %prec OR
AstBase* SteelParser::ReductionRuleHandler0040 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 298 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::OR,a,b); 
#line 1141 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 41: exp <- exp:a AND exp:b    %left %prec AND
AstBase* SteelParser::ReductionRuleHandler0041 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 300 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::AND,a,b); 
#line 1155 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 42: exp <- exp:a EQ exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0042 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 302 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::EQ,a,b); 
#line 1169 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 43: exp <- exp:a NE exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0043 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 304 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::NE,a,b); 
#line 1183 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 44: exp <- exp:a LT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0044 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 306 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LT,a,b); 
#line 1197 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 45: exp <- exp:a GT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0045 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 308 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GT,a,b); 
#line 1211 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 46: exp <- exp:a LTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0046 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 310 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LTE,a,b); 
#line 1225 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 47: exp <- exp:a GTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0047 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 312 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GTE,a,b); 
#line 1239 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 48: exp <- exp:a CAT exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0048 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 314 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::CAT,a,b); 
#line 1253 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 49: exp <- '(' exp:exp ')'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0049 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 316 "steel.trison"
 return exp; 
#line 1265 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 50: exp <- '-' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0050 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 318 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::MINUS,exp); 
#line 1277 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 51: exp <- '+' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0051 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 320 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::PLUS,exp); 
#line 1289 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 52: exp <- NOT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0052 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 322 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::NOT,exp); 
#line 1301 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 53: exp <- CAT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0053 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 324 "steel.trison"
 return new AstUnaryOp(exp->GetLine(),
exp->GetScript(), AstUnaryOp::CAT,exp); 
#line 1314 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 54: exp <- exp:lvalue '[' exp:index ']'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0054 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* index = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 327 "steel.trison"
 return new AstArrayElement(lvalue->GetLine(),lvalue->GetScript(),lvalue,index); 
#line 1328 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 55: exp <- INCREMENT exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0055 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 329 "steel.trison"
 return new AstIncrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstIncrement::PRE);
#line 1340 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 56: exp <- exp:lvalue INCREMENT     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0056 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 331 "steel.trison"
 return new AstIncrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstIncrement::POST);
#line 1352 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 57: exp <- DECREMENT exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0057 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 333 "steel.trison"
 return new AstDecrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstDecrement::PRE);
#line 1364 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 58: exp <- exp:lvalue DECREMENT     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0058 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 335 "steel.trison"
 return new AstDecrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstDecrement::POST);
#line 1376 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 59: exp <- POP exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0059 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 337 "steel.trison"
 return new AstPop(lvalue->GetLine(),lvalue->GetScript(),lvalue); 
#line 1388 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 60: exp_statement <- ';':semi    
AstBase* SteelParser::ReductionRuleHandler0060 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 342 "steel.trison"

			int line = semi->GetLine();
			std::string script = semi->GetScript(); 
			delete semi;
			return new AstExpression(line,script); 
		
#line 1405 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 61: exp_statement <- exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0061 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 349 "steel.trison"
 delete semi;  return exp; 
#line 1419 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 62: int_literal <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0062 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 354 "steel.trison"
 return i; 
#line 1431 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 63: var_identifier <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0063 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 359 "steel.trison"
 return id; 
#line 1443 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 64: func_identifier <- FUNC_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0064 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 364 "steel.trison"
 return id; 
#line 1455 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 65: array_identifier <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0065 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 369 "steel.trison"
 return id; 
#line 1467 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 66: call <- func_identifier:id '(' ')'    
AstBase* SteelParser::ReductionRuleHandler0066 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 375 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id);
#line 1479 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 67: call <- func_identifier:id '(' param_list:params ')'    
AstBase* SteelParser::ReductionRuleHandler0067 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstParamList* params = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 377 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id,params);
#line 1493 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 68: vardecl <- VAR var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0068 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 382 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id);
#line 1505 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 69: vardecl <- VAR var_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0069 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 384 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id,exp); 
#line 1519 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 70: vardecl <- VAR array_identifier:id '[' exp:i ']'    
AstBase* SteelParser::ReductionRuleHandler0070 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* i = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 386 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id,i); 
#line 1533 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 71: vardecl <- VAR array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0071 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 388 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id); 
#line 1545 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 72: vardecl <- VAR array_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0072 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 390 "steel.trison"

							AstArrayDeclaration *pDecl =  new AstArrayDeclaration(id->GetLine(),id->GetScript(),id);
							pDecl->assign(exp);
							return pDecl;
						 
#line 1563 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 73: param_list <- exp:exp    
AstBase* SteelParser::ReductionRuleHandler0073 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 399 "steel.trison"
 AstParamList * pList = new AstParamList ( exp->GetLine(), exp->GetScript() );
		  pList->add(exp);
		  return pList;
		
#line 1578 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 74: param_list <- param_list:list ',' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0074 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamList* list = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 404 "steel.trison"
 list->add(exp); return list;
#line 1592 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}



// ///////////////////////////////////////////////////////////////////////////
// reduction rule lookup table
// ///////////////////////////////////////////////////////////////////////////

SteelParser::ReductionRule const SteelParser::ms_reduction_rule[] =
{
    {                 Token::START_,  2, &SteelParser::ReductionRuleHandler0000, "rule 0: %start <- root END_    "},
    {                 Token::root__,  1, &SteelParser::ReductionRuleHandler0001, "rule 1: root <- statement_list    "},
    {      Token::func_definition__,  8, &SteelParser::ReductionRuleHandler0002, "rule 2: func_definition <- FUNCTION FUNC_IDENTIFIER '(' param_definition ')' '{' statement_list '}'    "},
    {      Token::func_definition__,  7, &SteelParser::ReductionRuleHandler0003, "rule 3: func_definition <- FUNCTION FUNC_IDENTIFIER '(' ')' '{' statement_list '}'    "},
    {      Token::func_definition__,  9, &SteelParser::ReductionRuleHandler0004, "rule 4: func_definition <- FINAL FUNCTION FUNC_IDENTIFIER '(' param_definition ')' '{' statement_list '}'    "},
    {      Token::func_definition__,  8, &SteelParser::ReductionRuleHandler0005, "rule 5: func_definition <- FINAL FUNCTION FUNC_IDENTIFIER '(' ')' '{' statement_list '}'    "},
    {             Token::param_id__,  1, &SteelParser::ReductionRuleHandler0006, "rule 6: param_id <- VAR_IDENTIFIER    "},
    {             Token::param_id__,  1, &SteelParser::ReductionRuleHandler0007, "rule 7: param_id <- ARRAY_IDENTIFIER    "},
    {     Token::param_definition__,  1, &SteelParser::ReductionRuleHandler0008, "rule 8: param_definition <- vardecl    "},
    {     Token::param_definition__,  3, &SteelParser::ReductionRuleHandler0009, "rule 9: param_definition <- param_definition ',' vardecl    "},
    {       Token::statement_list__,  1, &SteelParser::ReductionRuleHandler0010, "rule 10: statement_list <- statement    "},
    {       Token::statement_list__,  2, &SteelParser::ReductionRuleHandler0011, "rule 11: statement_list <- statement_list statement    "},
    {            Token::statement__,  1, &SteelParser::ReductionRuleHandler0012, "rule 12: statement <- exp_statement    "},
    {            Token::statement__,  1, &SteelParser::ReductionRuleHandler0013, "rule 13: statement <- func_definition    "},
    {            Token::statement__,  3, &SteelParser::ReductionRuleHandler0014, "rule 14: statement <- '{' statement_list '}'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0015, "rule 15: statement <- '{' '}'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0016, "rule 16: statement <- vardecl ';'    "},
    {            Token::statement__,  5, &SteelParser::ReductionRuleHandler0017, "rule 17: statement <- WHILE '(' exp ')' statement    "},
    {            Token::statement__,  7, &SteelParser::ReductionRuleHandler0018, "rule 18: statement <- IF '(' exp ')' statement ELSE statement     %prec ELSE"},
    {            Token::statement__,  5, &SteelParser::ReductionRuleHandler0019, "rule 19: statement <- IF '(' exp ')' statement     %prec NON_ELSE"},
    {            Token::statement__,  3, &SteelParser::ReductionRuleHandler0020, "rule 20: statement <- RETURN exp ';'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0021, "rule 21: statement <- RETURN ';'    "},
    {            Token::statement__,  6, &SteelParser::ReductionRuleHandler0022, "rule 22: statement <- FOR '(' exp_statement exp_statement ')' statement    "},
    {            Token::statement__,  7, &SteelParser::ReductionRuleHandler0023, "rule 23: statement <- FOR '(' exp_statement exp_statement exp ')' statement    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0024, "rule 24: statement <- BREAK ';'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0025, "rule 25: statement <- CONTINUE ';'    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0026, "rule 26: exp <- call    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0027, "rule 27: exp <- INT    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0028, "rule 28: exp <- FLOAT    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0029, "rule 29: exp <- STRING    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0030, "rule 30: exp <- var_identifier    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0031, "rule 31: exp <- array_identifier    "},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0032, "rule 32: exp <- exp '+' exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0033, "rule 33: exp <- exp '-' exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0034, "rule 34: exp <- exp '*' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0035, "rule 35: exp <- exp '/' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0036, "rule 36: exp <- exp '%' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0037, "rule 37: exp <- exp D exp    %left %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0038, "rule 38: exp <- exp '=' exp    %right %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0039, "rule 39: exp <- exp '^' exp    %right %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0040, "rule 40: exp <- exp OR exp    %left %prec OR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0041, "rule 41: exp <- exp AND exp    %left %prec AND"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0042, "rule 42: exp <- exp EQ exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0043, "rule 43: exp <- exp NE exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0044, "rule 44: exp <- exp LT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0045, "rule 45: exp <- exp GT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0046, "rule 46: exp <- exp LTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0047, "rule 47: exp <- exp GTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0048, "rule 48: exp <- exp CAT exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0049, "rule 49: exp <- '(' exp ')'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0050, "rule 50: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0051, "rule 51: exp <- '+' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0052, "rule 52: exp <- NOT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0053, "rule 53: exp <- CAT exp     %prec UNARY"},
    {                  Token::exp__,  4, &SteelParser::ReductionRuleHandler0054, "rule 54: exp <- exp '[' exp ']'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0055, "rule 55: exp <- INCREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0056, "rule 56: exp <- exp INCREMENT     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0057, "rule 57: exp <- DECREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0058, "rule 58: exp <- exp DECREMENT     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0059, "rule 59: exp <- POP exp     %prec UNARY"},
    {        Token::exp_statement__,  1, &SteelParser::ReductionRuleHandler0060, "rule 60: exp_statement <- ';'    "},
    {        Token::exp_statement__,  2, &SteelParser::ReductionRuleHandler0061, "rule 61: exp_statement <- exp ';'    "},
    {          Token::int_literal__,  1, &SteelParser::ReductionRuleHandler0062, "rule 62: int_literal <- INT    "},
    {       Token::var_identifier__,  1, &SteelParser::ReductionRuleHandler0063, "rule 63: var_identifier <- VAR_IDENTIFIER    "},
    {      Token::func_identifier__,  1, &SteelParser::ReductionRuleHandler0064, "rule 64: func_identifier <- FUNC_IDENTIFIER    "},
    {     Token::array_identifier__,  1, &SteelParser::ReductionRuleHandler0065, "rule 65: array_identifier <- ARRAY_IDENTIFIER    "},
    {                 Token::call__,  3, &SteelParser::ReductionRuleHandler0066, "rule 66: call <- func_identifier '(' ')'    "},
    {                 Token::call__,  4, &SteelParser::ReductionRuleHandler0067, "rule 67: call <- func_identifier '(' param_list ')'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0068, "rule 68: vardecl <- VAR var_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0069, "rule 69: vardecl <- VAR var_identifier '=' exp    "},
    {              Token::vardecl__,  5, &SteelParser::ReductionRuleHandler0070, "rule 70: vardecl <- VAR array_identifier '[' exp ']'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0071, "rule 71: vardecl <- VAR array_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0072, "rule 72: vardecl <- VAR array_identifier '=' exp    "},
    {           Token::param_list__,  1, &SteelParser::ReductionRuleHandler0073, "rule 73: param_list <- exp    "},
    {           Token::param_list__,  3, &SteelParser::ReductionRuleHandler0074, "rule 74: param_list <- param_list ',' exp    "},

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
    {   1,   25,    0,   26,   11}, // state    0
    {   0,    0,   37,    0,    0}, // state    1
    {  38,   14,    0,   52,    5}, // state    2
    {  57,   26,    0,   83,   10}, // state    3
    {  93,   14,    0,  107,    5}, // state    4
    { 112,   14,    0,  126,    5}, // state    5
    { 131,   14,    0,  145,    5}, // state    6
    { 150,    1,    0,    0,    0}, // state    7
    { 151,    1,    0,    0,    0}, // state    8
    { 152,    1,    0,    0,    0}, // state    9
    { 153,   15,    0,  168,    5}, // state   10
    { 173,    1,    0,    0,    0}, // state   11
    { 174,    1,    0,    0,    0}, // state   12
    {   0,    0,  175,    0,    0}, // state   13
    {   0,    0,  176,    0,    0}, // state   14
    {   0,    0,  177,    0,    0}, // state   15
    { 178,    1,    0,    0,    0}, // state   16
    { 179,    2,    0,  181,    2}, // state   17
    {   0,    0,  183,    0,    0}, // state   18
    {   0,    0,  184,    0,    0}, // state   19
    {   0,    0,  185,    0,    0}, // state   20
    { 186,   14,    0,  200,    5}, // state   21
    { 205,   14,    0,  219,    5}, // state   22
    { 224,   14,    0,  238,    5}, // state   23
    { 243,   14,    0,  257,    5}, // state   24
    { 262,    1,    0,    0,    0}, // state   25
    { 263,    1,    0,    0,    0}, // state   26
    {   0,    0,  264,    0,    0}, // state   27
    { 265,   25,  290,  291,    9}, // state   28
    {   0,    0,  300,    0,    0}, // state   29
    { 301,   21,    0,    0,    0}, // state   30
    {   0,    0,  322,    0,    0}, // state   31
    {   0,    0,  323,    0,    0}, // state   32
    { 324,    1,    0,    0,    0}, // state   33
    {   0,    0,  325,    0,    0}, // state   34
    {   0,    0,  326,    0,    0}, // state   35
    { 327,    1,    0,    0,    0}, // state   36
    { 328,   21,    0,    0,    0}, // state   37
    {   0,    0,  349,    0,    0}, // state   38
    { 350,   26,    0,  376,    9}, // state   39
    { 385,    5,  390,    0,    0}, // state   40
    { 391,    5,  396,    0,    0}, // state   41
    { 397,    5,  402,    0,    0}, // state   42
    { 403,   14,    0,  417,    5}, // state   43
    {   0,    0,  422,    0,    0}, // state   44
    {   0,    0,  423,    0,    0}, // state   45
    {   0,    0,  424,    0,    0}, // state   46
    { 425,   21,    0,    0,    0}, // state   47
    { 446,   14,    0,  460,    5}, // state   48
    { 465,    1,    0,    0,    0}, // state   49
    { 466,   15,    0,  481,    6}, // state   50
    { 487,    1,  488,    0,    0}, // state   51
    { 489,    2,  491,    0,    0}, // state   52
    { 492,    5,  497,    0,    0}, // state   53
    { 498,    5,  503,    0,    0}, // state   54
    { 504,    5,  509,    0,    0}, // state   55
    { 510,    5,  515,    0,    0}, // state   56
    { 516,    1,    0,    0,    0}, // state   57
    {   0,    0,  517,    0,    0}, // state   58
    {   0,    0,  518,    0,    0}, // state   59
    {   0,    0,  519,    0,    0}, // state   60
    { 520,   14,    0,  534,    5}, // state   61
    { 539,   14,    0,  553,    5}, // state   62
    { 558,   14,    0,  572,    5}, // state   63
    { 577,   14,    0,  591,    5}, // state   64
    { 596,   14,    0,  610,    5}, // state   65
    { 615,   14,    0,  629,    5}, // state   66
    { 634,   14,    0,  648,    5}, // state   67
    { 653,   14,    0,  667,    5}, // state   68
    { 672,   14,    0,  686,    5}, // state   69
    { 691,   14,    0,  705,    5}, // state   70
    { 710,   14,    0,  724,    5}, // state   71
    { 729,   14,    0,  743,    5}, // state   72
    { 748,   14,    0,  762,    5}, // state   73
    { 767,   14,    0,  781,    5}, // state   74
    { 786,   14,    0,  800,    5}, // state   75
    { 805,   14,    0,  819,    5}, // state   76
    { 824,   14,    0,  838,    5}, // state   77
    {   0,    0,  843,    0,    0}, // state   78
    {   0,    0,  844,    0,    0}, // state   79
    { 845,   14,    0,  859,    5}, // state   80
    { 864,   15,    0,  879,    6}, // state   81
    {   0,    0,  885,    0,    0}, // state   82
    {   0,    0,  886,    0,    0}, // state   83
    {   0,    0,  887,    0,    0}, // state   84
    { 888,   21,    0,    0,    0}, // state   85
    {   0,    0,  909,    0,    0}, // state   86
    { 910,   21,    0,    0,    0}, // state   87
    { 931,    2,    0,  933,    2}, // state   88
    { 935,   15,    0,  950,    6}, // state   89
    { 956,   14,    0,  970,    5}, // state   90
    { 975,   14,    0,  989,    5}, // state   91
    { 994,   14,    0, 1008,    5}, // state   92
    {1013,    1,    0,    0,    0}, // state   93
    {1014,   20, 1034,    0,    0}, // state   94
    {1035,   21,    0,    0,    0}, // state   95
    {1056,    8, 1064,    0,    0}, // state   96
    {1065,    8, 1073,    0,    0}, // state   97
    {1074,    5, 1079,    0,    0}, // state   98
    {1080,    5, 1085,    0,    0}, // state   99
    {1086,    4, 1090,    0,    0}, // state  100
    {1091,    5, 1096,    0,    0}, // state  101
    {1097,    4, 1101,    0,    0}, // state  102
    {1102,   11, 1113,    0,    0}, // state  103
    {1114,   11, 1125,    0,    0}, // state  104
    {1126,   15, 1141,    0,    0}, // state  105
    {1142,   15, 1157,    0,    0}, // state  106
    {1158,   11, 1169,    0,    0}, // state  107
    {1170,   11, 1181,    0,    0}, // state  108
    {1182,   17, 1199,    0,    0}, // state  109
    {1200,   18, 1218,    0,    0}, // state  110
    {1219,    8, 1227,    0,    0}, // state  111
    {   0,    0, 1228,    0,    0}, // state  112
    {1229,   20, 1249,    0,    0}, // state  113
    {1250,    2,    0,    0,    0}, // state  114
    {1252,   25,    0, 1277,    9}, // state  115
    {1286,   25,    0, 1311,    9}, // state  116
    {1320,    1,    0,    0,    0}, // state  117
    {1321,    2,    0,    0,    0}, // state  118
    {   0,    0, 1323,    0,    0}, // state  119
    {1324,   15,    0, 1339,    5}, // state  120
    {1344,   20, 1364,    0,    0}, // state  121
    {1365,   20, 1385,    0,    0}, // state  122
    {1386,   21,    0,    0,    0}, // state  123
    {1407,    2,    0, 1409,    2}, // state  124
    {   0,    0, 1411,    0,    0}, // state  125
    {   0,    0, 1412,    0,    0}, // state  126
    {1413,   14,    0, 1427,    5}, // state  127
    {   0,    0, 1432,    0,    0}, // state  128
    {1433,    1, 1434,    0,    0}, // state  129
    {1435,   25,    0, 1460,   10}, // state  130
    {1470,    1,    0,    0,    0}, // state  131
    {1471,    1,    0, 1472,    1}, // state  132
    {1473,   25,    0, 1498,    9}, // state  133
    {1507,   21,    0,    0,    0}, // state  134
    {   0,    0, 1528,    0,    0}, // state  135
    {1529,    1,    0,    0,    0}, // state  136
    {1530,    2,    0,    0,    0}, // state  137
    {1532,   20, 1552,    0,    0}, // state  138
    {1553,   25,    0, 1578,    9}, // state  139
    {1587,   26,    0, 1613,    9}, // state  140
    {1622,   25,    0, 1647,   10}, // state  141
    {   0,    0, 1657,    0,    0}, // state  142
    {   0,    0, 1658,    0,    0}, // state  143
    {1659,   25,    0, 1684,    9}, // state  144
    {1693,   25,    0, 1718,   10}, // state  145
    {1728,    1,    0,    0,    0}, // state  146
    {   0,    0, 1729,    0,    0}, // state  147
    {   0,    0, 1730,    0,    0}, // state  148
    {1731,   26,    0, 1757,    9}, // state  149
    {   0,    0, 1766,    0,    0}, // state  150
    {1767,   26,    0, 1793,    9}, // state  151
    {1802,   25,    0, 1827,   10}, // state  152
    {   0,    0, 1837,    0,    0}, // state  153
    {   0,    0, 1838,    0,    0}, // state  154
    {1839,   26,    0, 1865,    9}, // state  155
    {   0,    0, 1874,    0,    0}  // state  156

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
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {                 Token::root__, {                  TA_PUSH_STATE,   26}},
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   28}},
    {            Token::statement__, {                  TA_PUSH_STATE,   29}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   60}},

// ///////////////////////////////////////////////////////////////////////////
// state    2
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   37}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   38}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   39}},
    {            Token::statement__, {                  TA_PUSH_STATE,   29}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state    4
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   40}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state    5
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   41}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state    6
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   42}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   46}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   47}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   64}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   63}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   65}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    // nonterminal transitions
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   51}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state   18
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   27}},

// ///////////////////////////////////////////////////////////////////////////
// state   19
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   20
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state   21
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   53}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   22
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   54}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   55}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   56}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   57}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,   58}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   59}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   30}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   81}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   26}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   82}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   83}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   84}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   59}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   40
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state   41
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   51}},

// ///////////////////////////////////////////////////////////////////////////
// state   42
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state   43
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   85}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   44
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   45
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   25}},

// ///////////////////////////////////////////////////////////////////////////
// state   46
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   47
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   86}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   48
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   87}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   49
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   88}},

// ///////////////////////////////////////////////////////////////////////////
// state   50
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   89}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   51
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   90}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state   52
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   91}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   92}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   71}},

// ///////////////////////////////////////////////////////////////////////////
// state   53
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   55}},

// ///////////////////////////////////////////////////////////////////////////
// state   54
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   57}},

// ///////////////////////////////////////////////////////////////////////////
// state   55
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   53}},

// ///////////////////////////////////////////////////////////////////////////
// state   56
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   59}},

// ///////////////////////////////////////////////////////////////////////////
// state   57
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   93}},

// ///////////////////////////////////////////////////////////////////////////
// state   58
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state   59
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   60
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   61
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   94}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   62
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   95}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   63
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   96}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   64
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   97}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   65
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   98}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   66
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   99}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   67
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  100}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   68
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  101}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   69
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  102}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   70
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  103}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   71
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  104}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   72
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  105}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   73
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  106}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   74
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  107}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   75
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  108}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   76
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  109}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   77
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  110}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   78
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   56}},

// ///////////////////////////////////////////////////////////////////////////
// state   79
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   58}},

// ///////////////////////////////////////////////////////////////////////////
// state   80
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  111}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   81
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  112}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  113}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {           Token::param_list__, {                  TA_PUSH_STATE,  114}},

// ///////////////////////////////////////////////////////////////////////////
// state   82
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state   83
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   84
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state   85
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  115}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   86
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state   87
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  116}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   88
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  117}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,  118}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,  119}},

// ///////////////////////////////////////////////////////////////////////////
// state   89
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,  120}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   90
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  121}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   91
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  122}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   92
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  123}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   93
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,  124}},

// ///////////////////////////////////////////////////////////////////////////
// state   94
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state   95
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  125}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   96
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   97
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   98
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   99
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  100
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state  101
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  102
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state  103
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state  104
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state  105
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state  106
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state  107
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state  108
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state  109
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state  110
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state  111
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state  112
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state  113
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state  114
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  126}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  127}},

// ///////////////////////////////////////////////////////////////////////////
// state  115
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,  128}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  116
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,  129}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  117
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  130}},

// ///////////////////////////////////////////////////////////////////////////
// state  118
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  131}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  132}},

// ///////////////////////////////////////////////////////////////////////////
// state  119
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state  120
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  133}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  134}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  121
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   69}},

// ///////////////////////////////////////////////////////////////////////////
// state  122
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   72}},

// ///////////////////////////////////////////////////////////////////////////
// state  123
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  135}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state  124
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  136}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,  137}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,  119}},

// ///////////////////////////////////////////////////////////////////////////
// state  125
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   54}},

// ///////////////////////////////////////////////////////////////////////////
// state  126
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   67}},

// ///////////////////////////////////////////////////////////////////////////
// state  127
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  138}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  128
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state  129
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::ELSE, {        TA_SHIFT_AND_PUSH_STATE,  139}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state  130
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  140}},
    {            Token::statement__, {                  TA_PUSH_STATE,   29}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  131
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  141}},

// ///////////////////////////////////////////////////////////////////////////
// state  132
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {              Token::vardecl__, {                  TA_PUSH_STATE,  142}},

// ///////////////////////////////////////////////////////////////////////////
// state  133
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,  143}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  134
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  144}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state  135
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   70}},

// ///////////////////////////////////////////////////////////////////////////
// state  136
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  145}},

// ///////////////////////////////////////////////////////////////////////////
// state  137
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  146}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  132}},

// ///////////////////////////////////////////////////////////////////////////
// state  138
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   74}},

// ///////////////////////////////////////////////////////////////////////////
// state  139
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,  147}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  140
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  148}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   59}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  141
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  149}},
    {            Token::statement__, {                  TA_PUSH_STATE,   29}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  142
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state  143
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state  144
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,  150}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  145
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  151}},
    {            Token::statement__, {                  TA_PUSH_STATE,   29}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  146
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  152}},

// ///////////////////////////////////////////////////////////////////////////
// state  147
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state  148
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state  149
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  153}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   59}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  150
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state  151
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  154}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   59}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  152
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  155}},
    {            Token::statement__, {                  TA_PUSH_STATE,   29}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  153
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    2}},

// ///////////////////////////////////////////////////////////////////////////
// state  154
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    5}},

// ///////////////////////////////////////////////////////////////////////////
// state  155
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  156}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    {                  Token::FINAL, {        TA_SHIFT_AND_PUSH_STATE,   25}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   59}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   30}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   31}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   32}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   33}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   34}},
    {                 Token::call__, {                  TA_PUSH_STATE,   35}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state  156
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    4}}

};

unsigned int const SteelParser::ms_state_transition_count =
    sizeof(SteelParser::ms_state_transition) /
    sizeof(SteelParser::StateTransition);


#line 26 "steel.trison"


void SteelParser::setBuffer(const char * pBuffer, const std::string &name)
{	
	assert( NULL != m_scanner );
	m_scanner->setBuffer(pBuffer,name);
}


SteelParser::Token::Type SteelParser::Scan ()
{
	assert(m_scanner != NULL);
	return m_scanner->Scan(&m_lookahead_token);
}

#line 4639 "SteelParser.cpp"

