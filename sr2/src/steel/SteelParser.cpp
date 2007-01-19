#include "SteelParser.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

#define DEBUG_SPEW_1(x) if (m_debug_spew_level >= 1) std::cerr << x
#define DEBUG_SPEW_2(x) if (m_debug_spew_level >= 2) std::cerr << x


#line 21 "steel.trison"

	#include "SteelScanner.h"
	#include "Ast.h"

#line 17 "SteelParser.cpp"

SteelParser::SteelParser ()

{

#line 42 "steel.trison"

    m_scanner = new SteelScanner();

#line 27 "SteelParser.cpp"
    m_debug_spew_level = 0;
    DEBUG_SPEW_2("### number of state transitions = " << ms_state_transition_count << std::endl);
    m_reduction_token = static_cast<AstBase*>(0);
}

SteelParser::~SteelParser ()
{

#line 46 "steel.trison"

    delete m_scanner;

#line 40 "SteelParser.cpp"
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

#line 402 "SteelParser.cpp"
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

#line 100 "steel.trison"

				AstScript * pScript =   new AstScript(
							list->GetLine(),
							list->GetScript());
			        pScript->SetList(list);
				return pScript;
			
#line 514 "SteelParser.cpp"
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

#line 113 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									params,
									stmts);
				
#line 542 "SteelParser.cpp"
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

#line 124 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									NULL,
									stmts);
				
#line 568 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 4: param_id <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0004 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 138 "steel.trison"
 return id; 
#line 580 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 5: param_id <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0005 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 140 "steel.trison"
 return id; 
#line 592 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 6: param_definition <- vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0006 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 146 "steel.trison"

				AstParamDefinitionList * pList = new AstParamDefinitionList(decl->GetLine(),decl->GetScript());
				pList->add(static_cast<AstDeclaration*>(decl));
				return pList;		
			
#line 608 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 7: param_definition <- param_definition:list ',' vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0007 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamDefinitionList* list = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 153 "steel.trison"

				list->add(static_cast<AstDeclaration*>(decl));
				return list;
			
#line 625 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 8: statement_list <- statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0008 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 161 "steel.trison"

				AstStatementList *pList = new AstStatementList( stmt->GetLine(),
										stmt->GetScript());
				pList->add(stmt);
				return pList;
			
#line 642 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 9: statement_list <- statement_list:list statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0009 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 169 "steel.trison"

					list->add( stmt ); 
					return list;
				
#line 659 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 10: statement <- exp_statement:exp    
AstBase* SteelParser::ReductionRuleHandler0010 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 177 "steel.trison"
 return new AstExpressionStatement(exp->GetLine(),exp->GetScript(), exp); 
#line 671 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 11: statement <- func_definition:func    
AstBase* SteelParser::ReductionRuleHandler0011 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinition* func = static_cast< AstFunctionDefinition* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 179 "steel.trison"
 return func; 
#line 683 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 12: statement <- '{':b1 statement_list:list '}':b2    
AstBase* SteelParser::ReductionRuleHandler0012 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 181 "steel.trison"
 delete b1; delete b2; return list; 
#line 699 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 13: statement <- '{':b1 '}':b2    
AstBase* SteelParser::ReductionRuleHandler0013 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 183 "steel.trison"

			 int line = b1->GetLine();
			 std::string script = b1->GetScript();
			 delete b1;
			 delete b2;
			 return new AstStatement(line,script);
			
#line 719 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 14: statement <- vardecl:vardecl ';':semi    
AstBase* SteelParser::ReductionRuleHandler0014 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* vardecl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 191 "steel.trison"
 delete semi; return vardecl; 
#line 733 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 15: statement <- WHILE '(' exp:exp ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0015 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 193 "steel.trison"
 return new AstWhileStatement(exp->GetLine(), exp->GetScript(),exp,stmt); 
#line 747 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 16: statement <- IF '(' exp:exp ')' statement:stmt ELSE statement:elses     %prec ELSE
AstBase* SteelParser::ReductionRuleHandler0016 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);
    assert(6 < m_reduction_rule_token_count);
    AstStatement* elses = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);

#line 195 "steel.trison"
 return new AstIfStatement(exp->GetLine(), exp->GetScript(),exp,stmt,elses);
#line 763 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 17: statement <- IF '(' exp:exp ')' statement:stmt     %prec NON_ELSE
AstBase* SteelParser::ReductionRuleHandler0017 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 197 "steel.trison"
 return new AstIfStatement(exp->GetLine(),exp->GetScript(),exp,stmt); 
#line 777 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 18: statement <- RETURN exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0018 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 199 "steel.trison"
 delete semi; return new AstReturnStatement(exp->GetLine(),exp->GetScript(),exp);
#line 791 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 19: statement <- RETURN ';':semi    
AstBase* SteelParser::ReductionRuleHandler0019 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 202 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstReturnStatement(line,script);
			
#line 808 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 20: statement <- FOR '(' exp_statement:start exp_statement:condition ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0020 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* start = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* condition = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);
    assert(5 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 5]);

#line 210 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition, 
						new AstExpression (start->GetLine(),start->GetScript()), stmt); 
			
#line 827 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 21: statement <- FOR '(' exp_statement:start exp_statement:condition exp:iteration ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0021 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* start = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* condition = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);
    assert(4 < m_reduction_rule_token_count);
    AstExpression* iteration = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);
    assert(6 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);

#line 216 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition,iteration,stmt);
			
#line 847 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 22: statement <- BREAK ';':semi    
AstBase* SteelParser::ReductionRuleHandler0022 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 221 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstBreakStatement(line,script); 
			
#line 864 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 23: statement <- CONTINUE ';':semi    
AstBase* SteelParser::ReductionRuleHandler0023 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 229 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstContinueStatement(line,script); 
			
#line 881 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 24: exp <- call:call    
AstBase* SteelParser::ReductionRuleHandler0024 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstCallExpression* call = static_cast< AstCallExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 240 "steel.trison"
 return call; 
#line 893 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 25: exp <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0025 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 242 "steel.trison"
 return i;
#line 905 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 26: exp <- FLOAT:f    
AstBase* SteelParser::ReductionRuleHandler0026 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* f = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 244 "steel.trison"
 return f; 
#line 917 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 27: exp <- STRING:s    
AstBase* SteelParser::ReductionRuleHandler0027 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* s = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 246 "steel.trison"
 return s; 
#line 929 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 28: exp <- var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0028 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 248 "steel.trison"
 return id; 
#line 941 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 29: exp <- array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0029 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 250 "steel.trison"
 return id; 
#line 953 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 30: exp <- exp:a '+' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0030 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 252 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::ADD,a,b); 
#line 967 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 31: exp <- exp:a '-' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0031 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 254 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::SUB,a,b); 
#line 981 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 32: exp <- exp:a '*' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0032 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 256 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MULT,a,b); 
#line 995 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 33: exp <- exp:a '/' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0033 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 258 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::DIV,a,b); 
#line 1009 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 34: exp <- exp:a '%' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0034 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 260 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MOD,a,b); 
#line 1023 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 35: exp <- exp:a D exp:b    %left %prec POW
AstBase* SteelParser::ReductionRuleHandler0035 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 262 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::D,a,b); 
#line 1037 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 36: exp <- exp:lvalue '=' exp:exp    %right %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0036 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 264 "steel.trison"
 return new AstVarAssignmentExpression(lvalue->GetLine(),lvalue->GetScript(),lvalue,exp); 
#line 1051 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 37: exp <- exp:a '^' exp:b    %right %prec POW
AstBase* SteelParser::ReductionRuleHandler0037 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 266 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::POW,a,b); 
#line 1065 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 38: exp <- exp:a OR exp:b    %left %prec OR
AstBase* SteelParser::ReductionRuleHandler0038 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 268 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::OR,a,b); 
#line 1079 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 39: exp <- exp:a AND exp:b    %left %prec AND
AstBase* SteelParser::ReductionRuleHandler0039 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 270 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::AND,a,b); 
#line 1093 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 40: exp <- exp:a EQ exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0040 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 272 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::EQ,a,b); 
#line 1107 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 41: exp <- exp:a NE exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0041 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 274 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::NE,a,b); 
#line 1121 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 42: exp <- exp:a LT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0042 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 276 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LT,a,b); 
#line 1135 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 43: exp <- exp:a GT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0043 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 278 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GT,a,b); 
#line 1149 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 44: exp <- exp:a LTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0044 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 280 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LTE,a,b); 
#line 1163 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 45: exp <- exp:a GTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0045 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 282 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GTE,a,b); 
#line 1177 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 46: exp <- '(' exp:exp ')'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0046 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 284 "steel.trison"
 return exp; 
#line 1189 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 47: exp <- '-' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0047 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 286 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::MINUS,exp); 
#line 1201 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 48: exp <- '+' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0048 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 288 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::PLUS,exp); 
#line 1213 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 49: exp <- NOT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0049 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 290 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::NOT,exp); 
#line 1225 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 50: exp <- exp:lvalue '[' exp:index ']'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0050 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* index = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 292 "steel.trison"
 return new AstArrayElement(lvalue->GetLine(),lvalue->GetScript(),lvalue,index); 
#line 1239 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 51: exp <- INCREMENT exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0051 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 294 "steel.trison"
 return new AstIncrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstIncrement::PRE);
#line 1251 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 52: exp <- exp:lvalue INCREMENT     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0052 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 296 "steel.trison"
 return new AstIncrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstIncrement::POST);
#line 1263 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 53: exp <- DECREMENT exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0053 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 298 "steel.trison"
 return new AstDecrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstDecrement::PRE);
#line 1275 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 54: exp <- exp:lvalue DECREMENT     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0054 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 300 "steel.trison"
 return new AstDecrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstDecrement::POST);
#line 1287 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 55: exp_statement <- ';':semi    
AstBase* SteelParser::ReductionRuleHandler0055 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 305 "steel.trison"

			int line = semi->GetLine();
			std::string script = semi->GetScript(); 
			delete semi;
			return new AstExpression(line,script); 
		
#line 1304 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 56: exp_statement <- exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0056 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 312 "steel.trison"
 delete semi;  return exp; 
#line 1318 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 57: int_literal <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0057 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 317 "steel.trison"
 return i; 
#line 1330 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 58: var_identifier <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0058 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 322 "steel.trison"
 return id; 
#line 1342 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 59: func_identifier <- FUNC_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0059 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 327 "steel.trison"
 return id; 
#line 1354 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 60: array_identifier <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0060 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 332 "steel.trison"
 return id; 
#line 1366 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 61: call <- func_identifier:id '(' ')'    
AstBase* SteelParser::ReductionRuleHandler0061 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 338 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id);
#line 1378 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 62: call <- func_identifier:id '(' param_list:params ')'    
AstBase* SteelParser::ReductionRuleHandler0062 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstParamList* params = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 340 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id,params);
#line 1392 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 63: vardecl <- VAR var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0063 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 345 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id);
#line 1404 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 64: vardecl <- VAR var_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0064 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 347 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id,exp); 
#line 1418 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 65: vardecl <- VAR array_identifier:id '[' exp:i ']'    
AstBase* SteelParser::ReductionRuleHandler0065 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* i = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 349 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id,i); 
#line 1432 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 66: vardecl <- VAR array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0066 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 351 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id); 
#line 1444 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 67: vardecl <- VAR array_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0067 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 353 "steel.trison"

							AstArrayDeclaration *pDecl =  new AstArrayDeclaration(id->GetLine(),id->GetScript(),id);
							pDecl->assign(exp);
							return pDecl;
						 
#line 1462 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 68: param_list <- exp:exp    
AstBase* SteelParser::ReductionRuleHandler0068 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 362 "steel.trison"
 AstParamList * pList = new AstParamList ( exp->GetLine(), exp->GetScript() );
		  pList->add(exp);
		  return pList;
		
#line 1477 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 69: param_list <- param_list:list ',' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0069 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamList* list = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 367 "steel.trison"
 list->add(exp); return list;
#line 1491 "SteelParser.cpp"
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
    {             Token::param_id__,  1, &SteelParser::ReductionRuleHandler0004, "rule 4: param_id <- VAR_IDENTIFIER    "},
    {             Token::param_id__,  1, &SteelParser::ReductionRuleHandler0005, "rule 5: param_id <- ARRAY_IDENTIFIER    "},
    {     Token::param_definition__,  1, &SteelParser::ReductionRuleHandler0006, "rule 6: param_definition <- vardecl    "},
    {     Token::param_definition__,  3, &SteelParser::ReductionRuleHandler0007, "rule 7: param_definition <- param_definition ',' vardecl    "},
    {       Token::statement_list__,  1, &SteelParser::ReductionRuleHandler0008, "rule 8: statement_list <- statement    "},
    {       Token::statement_list__,  2, &SteelParser::ReductionRuleHandler0009, "rule 9: statement_list <- statement_list statement    "},
    {            Token::statement__,  1, &SteelParser::ReductionRuleHandler0010, "rule 10: statement <- exp_statement    "},
    {            Token::statement__,  1, &SteelParser::ReductionRuleHandler0011, "rule 11: statement <- func_definition    "},
    {            Token::statement__,  3, &SteelParser::ReductionRuleHandler0012, "rule 12: statement <- '{' statement_list '}'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0013, "rule 13: statement <- '{' '}'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0014, "rule 14: statement <- vardecl ';'    "},
    {            Token::statement__,  5, &SteelParser::ReductionRuleHandler0015, "rule 15: statement <- WHILE '(' exp ')' statement    "},
    {            Token::statement__,  7, &SteelParser::ReductionRuleHandler0016, "rule 16: statement <- IF '(' exp ')' statement ELSE statement     %prec ELSE"},
    {            Token::statement__,  5, &SteelParser::ReductionRuleHandler0017, "rule 17: statement <- IF '(' exp ')' statement     %prec NON_ELSE"},
    {            Token::statement__,  3, &SteelParser::ReductionRuleHandler0018, "rule 18: statement <- RETURN exp ';'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0019, "rule 19: statement <- RETURN ';'    "},
    {            Token::statement__,  6, &SteelParser::ReductionRuleHandler0020, "rule 20: statement <- FOR '(' exp_statement exp_statement ')' statement    "},
    {            Token::statement__,  7, &SteelParser::ReductionRuleHandler0021, "rule 21: statement <- FOR '(' exp_statement exp_statement exp ')' statement    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0022, "rule 22: statement <- BREAK ';'    "},
    {            Token::statement__,  2, &SteelParser::ReductionRuleHandler0023, "rule 23: statement <- CONTINUE ';'    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0024, "rule 24: exp <- call    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0025, "rule 25: exp <- INT    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0026, "rule 26: exp <- FLOAT    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0027, "rule 27: exp <- STRING    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0028, "rule 28: exp <- var_identifier    "},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0029, "rule 29: exp <- array_identifier    "},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0030, "rule 30: exp <- exp '+' exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0031, "rule 31: exp <- exp '-' exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0032, "rule 32: exp <- exp '*' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0033, "rule 33: exp <- exp '/' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0034, "rule 34: exp <- exp '%' exp    %left %prec MULT_DIV_MOD"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0035, "rule 35: exp <- exp D exp    %left %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0036, "rule 36: exp <- exp '=' exp    %right %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0037, "rule 37: exp <- exp '^' exp    %right %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0038, "rule 38: exp <- exp OR exp    %left %prec OR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0039, "rule 39: exp <- exp AND exp    %left %prec AND"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0040, "rule 40: exp <- exp EQ exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0041, "rule 41: exp <- exp NE exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0042, "rule 42: exp <- exp LT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0043, "rule 43: exp <- exp GT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0044, "rule 44: exp <- exp LTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0045, "rule 45: exp <- exp GTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0046, "rule 46: exp <- '(' exp ')'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0047, "rule 47: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0048, "rule 48: exp <- '+' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0049, "rule 49: exp <- NOT exp     %prec UNARY"},
    {                  Token::exp__,  4, &SteelParser::ReductionRuleHandler0050, "rule 50: exp <- exp '[' exp ']'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0051, "rule 51: exp <- INCREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0052, "rule 52: exp <- exp INCREMENT     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0053, "rule 53: exp <- DECREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0054, "rule 54: exp <- exp DECREMENT     %prec PAREN"},
    {        Token::exp_statement__,  1, &SteelParser::ReductionRuleHandler0055, "rule 55: exp_statement <- ';'    "},
    {        Token::exp_statement__,  2, &SteelParser::ReductionRuleHandler0056, "rule 56: exp_statement <- exp ';'    "},
    {          Token::int_literal__,  1, &SteelParser::ReductionRuleHandler0057, "rule 57: int_literal <- INT    "},
    {       Token::var_identifier__,  1, &SteelParser::ReductionRuleHandler0058, "rule 58: var_identifier <- VAR_IDENTIFIER    "},
    {      Token::func_identifier__,  1, &SteelParser::ReductionRuleHandler0059, "rule 59: func_identifier <- FUNC_IDENTIFIER    "},
    {     Token::array_identifier__,  1, &SteelParser::ReductionRuleHandler0060, "rule 60: array_identifier <- ARRAY_IDENTIFIER    "},
    {                 Token::call__,  3, &SteelParser::ReductionRuleHandler0061, "rule 61: call <- func_identifier '(' ')'    "},
    {                 Token::call__,  4, &SteelParser::ReductionRuleHandler0062, "rule 62: call <- func_identifier '(' param_list ')'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0063, "rule 63: vardecl <- VAR var_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0064, "rule 64: vardecl <- VAR var_identifier '=' exp    "},
    {              Token::vardecl__,  5, &SteelParser::ReductionRuleHandler0065, "rule 65: vardecl <- VAR array_identifier '[' exp ']'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0066, "rule 66: vardecl <- VAR array_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0067, "rule 67: vardecl <- VAR array_identifier '=' exp    "},
    {           Token::param_list__,  1, &SteelParser::ReductionRuleHandler0068, "rule 68: param_list <- exp    "},
    {           Token::param_list__,  3, &SteelParser::ReductionRuleHandler0069, "rule 69: param_list <- param_list ',' exp    "},

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
    {   1,   22,    0,   23,   11}, // state    0
    {   0,    0,   34,    0,    0}, // state    1
    {  35,   12,    0,   47,    5}, // state    2
    {  52,   23,    0,   75,   10}, // state    3
    {  85,   12,    0,   97,    5}, // state    4
    { 102,   12,    0,  114,    5}, // state    5
    { 119,   12,    0,  131,    5}, // state    6
    { 136,    1,    0,    0,    0}, // state    7
    { 137,    1,    0,    0,    0}, // state    8
    { 138,    1,    0,    0,    0}, // state    9
    { 139,   13,    0,  152,    5}, // state   10
    { 157,    1,    0,    0,    0}, // state   11
    { 158,    1,    0,    0,    0}, // state   12
    {   0,    0,  159,    0,    0}, // state   13
    {   0,    0,  160,    0,    0}, // state   14
    {   0,    0,  161,    0,    0}, // state   15
    { 162,    1,    0,    0,    0}, // state   16
    { 163,    2,    0,  165,    2}, // state   17
    {   0,    0,  167,    0,    0}, // state   18
    {   0,    0,  168,    0,    0}, // state   19
    {   0,    0,  169,    0,    0}, // state   20
    { 170,   12,    0,  182,    5}, // state   21
    { 187,   12,    0,  199,    5}, // state   22
    { 204,    1,    0,    0,    0}, // state   23
    {   0,    0,  205,    0,    0}, // state   24
    { 206,   22,  228,  229,    9}, // state   25
    {   0,    0,  238,    0,    0}, // state   26
    { 239,   20,    0,    0,    0}, // state   27
    {   0,    0,  259,    0,    0}, // state   28
    {   0,    0,  260,    0,    0}, // state   29
    { 261,    1,    0,    0,    0}, // state   30
    {   0,    0,  262,    0,    0}, // state   31
    {   0,    0,  263,    0,    0}, // state   32
    { 264,    1,    0,    0,    0}, // state   33
    { 265,   20,    0,    0,    0}, // state   34
    {   0,    0,  285,    0,    0}, // state   35
    { 286,   23,    0,  309,    9}, // state   36
    { 318,    5,  323,    0,    0}, // state   37
    { 324,    5,  329,    0,    0}, // state   38
    { 330,    5,  335,    0,    0}, // state   39
    { 336,   12,    0,  348,    5}, // state   40
    {   0,    0,  353,    0,    0}, // state   41
    {   0,    0,  354,    0,    0}, // state   42
    {   0,    0,  355,    0,    0}, // state   43
    { 356,   20,    0,    0,    0}, // state   44
    { 376,   12,    0,  388,    5}, // state   45
    { 393,    1,    0,    0,    0}, // state   46
    { 394,   13,    0,  407,    6}, // state   47
    { 413,    1,  414,    0,    0}, // state   48
    { 415,    2,  417,    0,    0}, // state   49
    { 418,    5,  423,    0,    0}, // state   50
    { 424,    5,  429,    0,    0}, // state   51
    {   0,    0,  430,    0,    0}, // state   52
    {   0,    0,  431,    0,    0}, // state   53
    {   0,    0,  432,    0,    0}, // state   54
    { 433,   12,    0,  445,    5}, // state   55
    { 450,   12,    0,  462,    5}, // state   56
    { 467,   12,    0,  479,    5}, // state   57
    { 484,   12,    0,  496,    5}, // state   58
    { 501,   12,    0,  513,    5}, // state   59
    { 518,   12,    0,  530,    5}, // state   60
    { 535,   12,    0,  547,    5}, // state   61
    { 552,   12,    0,  564,    5}, // state   62
    { 569,   12,    0,  581,    5}, // state   63
    { 586,   12,    0,  598,    5}, // state   64
    { 603,   12,    0,  615,    5}, // state   65
    { 620,   12,    0,  632,    5}, // state   66
    { 637,   12,    0,  649,    5}, // state   67
    { 654,   12,    0,  666,    5}, // state   68
    { 671,   12,    0,  683,    5}, // state   69
    { 688,   12,    0,  700,    5}, // state   70
    { 705,   12,    0,  717,    5}, // state   71
    {   0,    0,  722,    0,    0}, // state   72
    {   0,    0,  723,    0,    0}, // state   73
    { 724,   13,    0,  737,    6}, // state   74
    {   0,    0,  743,    0,    0}, // state   75
    {   0,    0,  744,    0,    0}, // state   76
    {   0,    0,  745,    0,    0}, // state   77
    { 746,   20,    0,    0,    0}, // state   78
    {   0,    0,  766,    0,    0}, // state   79
    { 767,   20,    0,    0,    0}, // state   80
    { 787,    2,    0,  789,    2}, // state   81
    { 791,   13,    0,  804,    6}, // state   82
    { 810,   12,    0,  822,    5}, // state   83
    { 827,   12,    0,  839,    5}, // state   84
    { 844,   12,    0,  856,    5}, // state   85
    { 861,   19,  880,    0,    0}, // state   86
    { 881,   20,    0,    0,    0}, // state   87
    { 901,    8,  909,    0,    0}, // state   88
    { 910,    8,  918,    0,    0}, // state   89
    { 919,    5,  924,    0,    0}, // state   90
    { 925,    5,  930,    0,    0}, // state   91
    { 931,    4,  935,    0,    0}, // state   92
    { 936,    5,  941,    0,    0}, // state   93
    { 942,    4,  946,    0,    0}, // state   94
    { 947,   10,  957,    0,    0}, // state   95
    { 958,   10,  968,    0,    0}, // state   96
    { 969,   14,  983,    0,    0}, // state   97
    { 984,   14,  998,    0,    0}, // state   98
    { 999,   10, 1009,    0,    0}, // state   99
    {1010,   10, 1020,    0,    0}, // state  100
    {1021,   16, 1037,    0,    0}, // state  101
    {1038,   17, 1055,    0,    0}, // state  102
    {   0,    0, 1056,    0,    0}, // state  103
    {1057,   19, 1076,    0,    0}, // state  104
    {1077,    2,    0,    0,    0}, // state  105
    {1079,   22,    0, 1101,    9}, // state  106
    {1110,   22,    0, 1132,    9}, // state  107
    {1141,    1,    0,    0,    0}, // state  108
    {1142,    2,    0,    0,    0}, // state  109
    {   0,    0, 1144,    0,    0}, // state  110
    {1145,   13,    0, 1158,    5}, // state  111
    {1163,   19, 1182,    0,    0}, // state  112
    {1183,   19, 1202,    0,    0}, // state  113
    {1203,   20,    0,    0,    0}, // state  114
    {   0,    0, 1223,    0,    0}, // state  115
    {   0,    0, 1224,    0,    0}, // state  116
    {1225,   12,    0, 1237,    5}, // state  117
    {   0,    0, 1242,    0,    0}, // state  118
    {1243,    1, 1244,    0,    0}, // state  119
    {1245,   22,    0, 1267,   10}, // state  120
    {1277,    1,    0,    0,    0}, // state  121
    {1278,    1,    0, 1279,    1}, // state  122
    {1280,   22,    0, 1302,    9}, // state  123
    {1311,   20,    0,    0,    0}, // state  124
    {   0,    0, 1331,    0,    0}, // state  125
    {1332,   19, 1351,    0,    0}, // state  126
    {1352,   22,    0, 1374,    9}, // state  127
    {1383,   23,    0, 1406,    9}, // state  128
    {1415,   22,    0, 1437,   10}, // state  129
    {   0,    0, 1447,    0,    0}, // state  130
    {   0,    0, 1448,    0,    0}, // state  131
    {1449,   22,    0, 1471,    9}, // state  132
    {   0,    0, 1480,    0,    0}, // state  133
    {   0,    0, 1481,    0,    0}, // state  134
    {1482,   23,    0, 1505,    9}, // state  135
    {   0,    0, 1514,    0,    0}, // state  136
    {   0,    0, 1515,    0,    0}  // state  137

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
    // nonterminal transitions
    {                 Token::root__, {                  TA_PUSH_STATE,   23}},
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   25}},
    {            Token::statement__, {                  TA_PUSH_STATE,   26}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   55}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   34}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   35}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   36}},
    {            Token::statement__, {                  TA_PUSH_STATE,   26}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   37}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   38}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   39}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   43}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   44}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   59}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   58}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   60}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    // nonterminal transitions
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   48}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   18
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   25}},

// ///////////////////////////////////////////////////////////////////////////
// state   19
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   26}},

// ///////////////////////////////////////////////////////////////////////////
// state   20
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   27}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   50}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   51}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
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
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,   53}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   74}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   75}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   77}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,   53}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   40
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   78}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   41
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state   42
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state   43
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   44
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   79}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state   45
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   80}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   46
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   81}},

// ///////////////////////////////////////////////////////////////////////////
// state   47
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   82}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   48
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   83}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   63}},

// ///////////////////////////////////////////////////////////////////////////
// state   49
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   84}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   85}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   50
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   51}},

// ///////////////////////////////////////////////////////////////////////////
// state   51
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   53}},

// ///////////////////////////////////////////////////////////////////////////
// state   52
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state   53
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state   54
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   56}},

// ///////////////////////////////////////////////////////////////////////////
// state   55
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   86}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   56
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   87}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   57
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   88}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   58
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   89}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   59
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   90}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   60
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   91}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   92}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   93}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   94}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   95}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   96}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   97}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   98}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   99}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  100}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  101}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  102}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   72
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state   73
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   54}},

// ///////////////////////////////////////////////////////////////////////////
// state   74
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  103}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  104}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {           Token::param_list__, {                  TA_PUSH_STATE,  105}},

// ///////////////////////////////////////////////////////////////////////////
// state   75
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state   76
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state   77
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   78
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  106}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state   79
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   80
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  107}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state   81
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  108}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,  109}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,  110}},

// ///////////////////////////////////////////////////////////////////////////
// state   82
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,  111}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   83
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  112}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   84
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  113}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   85
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  114}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   86
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   87
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  115}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state   88
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   89
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   30}},

// ///////////////////////////////////////////////////////////////////////////
// state   90
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   91
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   92
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state   93
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   94
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   95
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state   96
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state   97
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state   98
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state   99
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state  100
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state  101
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state  102
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state  103
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state  104
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state  105
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  116}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  117}},

// ///////////////////////////////////////////////////////////////////////////
// state  106
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,  118}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  107
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,  119}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  108
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  120}},

// ///////////////////////////////////////////////////////////////////////////
// state  109
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  121}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  122}},

// ///////////////////////////////////////////////////////////////////////////
// state  110
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state  111
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  123}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  124}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  112
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   64}},

// ///////////////////////////////////////////////////////////////////////////
// state  113
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   67}},

// ///////////////////////////////////////////////////////////////////////////
// state  114
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  125}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state  115
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state  116
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   62}},

// ///////////////////////////////////////////////////////////////////////////
// state  117
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  126}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  118
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state  119
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::ELSE, {        TA_SHIFT_AND_PUSH_STATE,  127}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state  120
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  128}},
    {            Token::statement__, {                  TA_PUSH_STATE,   26}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  121
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  129}},

// ///////////////////////////////////////////////////////////////////////////
// state  122
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {              Token::vardecl__, {                  TA_PUSH_STATE,  130}},

// ///////////////////////////////////////////////////////////////////////////
// state  123
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,  131}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  124
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  132}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},

// ///////////////////////////////////////////////////////////////////////////
// state  125
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   65}},

// ///////////////////////////////////////////////////////////////////////////
// state  126
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   69}},

// ///////////////////////////////////////////////////////////////////////////
// state  127
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,  133}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  128
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  134}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,   53}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  129
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  135}},
    {            Token::statement__, {                  TA_PUSH_STATE,   26}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  130
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state  131
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state  132
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,  136}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  133
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state  134
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state  135
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  137}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,   53}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   27}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   28}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   29}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   30}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  136
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state  137
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    2}}

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

#line 4049 "SteelParser.cpp"

