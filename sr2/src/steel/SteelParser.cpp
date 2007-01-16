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
        "ELSE",
        "EQ",
        "FLOAT",
        "FOR",
        "FUNCTION",
        "FUNC_IDENTIFIER",
        "GT",
        "GTE",
        "IF",
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

        "array_element_lvalue",
        "array_identifier",
        "call",
        "exp",
        "exp_statement",
        "func_definition",
        "func_definition_list",
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

#line 92 "steel.trison"

				AstScript * pScript =   new AstScript(
							list->GetLine(),
							list->GetScript());
			        pScript->SetList(list);
				return pScript;
			
#line 509 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 2: root <- func_definition_list:list    
AstBase* SteelParser::ReductionRuleHandler0002 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinitionList* list = static_cast< AstFunctionDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 101 "steel.trison"

				AstScript * pScript 
				= new AstScript(
						list->GetLine(),
						list->GetScript());
				pScript->SetFunctionList( list );
				return pScript;
			
#line 528 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 3: func_definition_list <- func_definition:def    
AstBase* SteelParser::ReductionRuleHandler0003 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinition* def = static_cast< AstFunctionDefinition* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 113 "steel.trison"

					AstFunctionDefinitionList * pList 
					= new AstFunctionDefinitionList(
						def->GetLine(),
						def->GetScript());
					pList->add(def);
					return pList;
				
#line 547 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 4: func_definition_list <- func_definition_list:list func_definition:def    
AstBase* SteelParser::ReductionRuleHandler0004 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinitionList* list = static_cast< AstFunctionDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstFunctionDefinition* def = static_cast< AstFunctionDefinition* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 123 "steel.trison"

					list->add(def);
					return list;
				
#line 564 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 5: func_definition <- FUNCTION FUNC_IDENTIFIER:id '(' param_definition:params ')' '{' statement_list:stmts '}'    
AstBase* SteelParser::ReductionRuleHandler0005 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstParamDefinitionList* params = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);
    assert(6 < m_reduction_rule_token_count);
    AstStatementList* stmts = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 6]);

#line 132 "steel.trison"

					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									params,
									stmts);
				
#line 586 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 6: func_definition <- FUNCTION FUNC_IDENTIFIER:id '(' ')' '{' statement_list:stmts '}'    
AstBase* SteelParser::ReductionRuleHandler0006 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(5 < m_reduction_rule_token_count);
    AstStatementList* stmts = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 5]);

#line 141 "steel.trison"

					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									NULL,
									stmts);
				
#line 606 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 7: param_id <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0007 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 153 "steel.trison"
 return id; 
#line 618 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 8: param_id <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0008 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 155 "steel.trison"
 return id; 
#line 630 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 9: param_definition <- vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0009 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 161 "steel.trison"

				AstParamDefinitionList * pList = new AstParamDefinitionList(decl->GetLine(),decl->GetScript());
				pList->add(static_cast<AstDeclaration*>(decl));
				return pList;		
			
#line 646 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 10: param_definition <- param_definition:list ',' vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0010 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamDefinitionList* list = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 168 "steel.trison"

				list->add(static_cast<AstDeclaration*>(decl));
				return list;
			
#line 663 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 11: statement_list <- statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0011 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 176 "steel.trison"

				AstStatementList *pList = new AstStatementList( stmt->GetLine(),
										stmt->GetScript());
				pList->add(stmt);
				return pList;
			
#line 680 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 12: statement_list <- statement_list:list statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0012 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 184 "steel.trison"

					list->add( stmt ); 
					return list;
				
#line 697 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 13: statement <- exp_statement:exp    
AstBase* SteelParser::ReductionRuleHandler0013 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 192 "steel.trison"
 return new AstExpressionStatement(exp->GetLine(),exp->GetScript(), exp); 
#line 709 "SteelParser.cpp"
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

#line 194 "steel.trison"
 delete b1; delete b2; return list; 
#line 725 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 15: statement <- '{':b1 '}':b2    
AstBase* SteelParser::ReductionRuleHandler0015 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 196 "steel.trison"

			 int line = b1->GetLine();
			 std::string script = b1->GetScript();
			 delete b1;
			 delete b2;
			 return new AstStatement(line,script);
			
#line 745 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 16: statement <- vardecl:vardecl ';':semi    
AstBase* SteelParser::ReductionRuleHandler0016 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* vardecl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 204 "steel.trison"
 delete semi; return vardecl; 
#line 759 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 17: statement <- WHILE '(' exp:exp ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0017 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 206 "steel.trison"
 return new AstWhileStatement(exp->GetLine(), exp->GetScript(),exp,stmt); 
#line 773 "SteelParser.cpp"
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

#line 208 "steel.trison"
 return new AstIfStatement(exp->GetLine(), exp->GetScript(),exp,stmt,elses);
#line 789 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 19: statement <- IF '(' exp:exp ')' statement:stmt     %prec NON_ELSE
AstBase* SteelParser::ReductionRuleHandler0019 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 210 "steel.trison"
 return new AstIfStatement(exp->GetLine(),exp->GetScript(),exp,stmt); 
#line 803 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 20: statement <- RETURN exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0020 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 212 "steel.trison"
 delete semi; return new AstReturnStatement(exp->GetLine(),exp->GetScript(),exp);
#line 817 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 21: statement <- RETURN ';':semi    
AstBase* SteelParser::ReductionRuleHandler0021 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 215 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstReturnStatement(line,script);
			
#line 834 "SteelParser.cpp"
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

#line 223 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition, 
						new AstExpression (start->GetLine(),start->GetScript()), stmt); 
			
#line 853 "SteelParser.cpp"
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

#line 229 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition,iteration,stmt);
			
#line 873 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 24: statement <- BREAK ';':semi    
AstBase* SteelParser::ReductionRuleHandler0024 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 234 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstBreakStatement(line,script); 
			
#line 890 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 25: statement <- CONTINUE ';':semi    
AstBase* SteelParser::ReductionRuleHandler0025 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 242 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstContinueStatement(line,script); 
			
#line 907 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 26: exp <- call:call    
AstBase* SteelParser::ReductionRuleHandler0026 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstCallExpression* call = static_cast< AstCallExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 253 "steel.trison"
 return call; 
#line 919 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 27: exp <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0027 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 255 "steel.trison"
 return i;
#line 931 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 28: exp <- FLOAT:f    
AstBase* SteelParser::ReductionRuleHandler0028 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* f = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 257 "steel.trison"
 return f; 
#line 943 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 29: exp <- STRING:s    
AstBase* SteelParser::ReductionRuleHandler0029 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* s = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 259 "steel.trison"
 return s; 
#line 955 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 30: exp <- var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0030 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 261 "steel.trison"
 return id; 
#line 967 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 31: exp <- array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0031 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 263 "steel.trison"
 return id; 
#line 979 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 32: exp <- exp:a '+' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0032 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 265 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::ADD,a,b); 
#line 993 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 33: exp <- exp:a '-' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0033 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 267 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::SUB,a,b); 
#line 1007 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 34: exp <- exp:a '*' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0034 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 269 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MULT,a,b); 
#line 1021 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 35: exp <- exp:a '/' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0035 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 271 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::DIV,a,b); 
#line 1035 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 36: exp <- exp:a '%' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0036 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 273 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MOD,a,b); 
#line 1049 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 37: exp <- exp:a D exp:b    %left %prec POW
AstBase* SteelParser::ReductionRuleHandler0037 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 275 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::D,a,b); 
#line 1063 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 38: exp <- var_identifier:id '=' exp:exp     %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0038 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 277 "steel.trison"
 return new AstVarAssignmentExpression(id->GetLine(),id->GetScript(),id,exp); 
#line 1077 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 39: exp <- array_element_lvalue:id '=' exp:exp     %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0039 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayElement* id = static_cast< AstArrayElement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 279 "steel.trison"
 return new AstArrayElementAssignmentExpression(id->GetLine(),id->GetScript(),id,exp); 
#line 1091 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 40: exp <- array_identifier:id '=' exp:exp     %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0040 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 281 "steel.trison"
 return new AstArrayAssignmentExpression(id->GetLine(),id->GetScript(),id,exp); 
#line 1105 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 41: exp <- exp:a '^' exp:b    %right %prec POW
AstBase* SteelParser::ReductionRuleHandler0041 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 283 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::POW,a,b); 
#line 1119 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 42: exp <- exp:a OR exp:b    %left %prec OR
AstBase* SteelParser::ReductionRuleHandler0042 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 285 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::OR,a,b); 
#line 1133 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 43: exp <- exp:a AND exp:b    %left %prec AND
AstBase* SteelParser::ReductionRuleHandler0043 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 287 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::AND,a,b); 
#line 1147 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 44: exp <- exp:a EQ exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0044 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 289 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::EQ,a,b); 
#line 1161 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 45: exp <- exp:a NE exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0045 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 291 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::NE,a,b); 
#line 1175 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 46: exp <- exp:a LT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0046 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 293 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LT,a,b); 
#line 1189 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 47: exp <- exp:a GT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0047 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 295 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GT,a,b); 
#line 1203 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 48: exp <- exp:a LTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0048 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 297 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LTE,a,b); 
#line 1217 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 49: exp <- exp:a GTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0049 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 299 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GTE,a,b); 
#line 1231 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 50: exp <- '(' exp:exp ')'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0050 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 301 "steel.trison"
 return exp; 
#line 1243 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 51: exp <- '-' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0051 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 303 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::MINUS,exp); 
#line 1255 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 52: exp <- '+' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0052 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 305 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::PLUS,exp); 
#line 1267 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 53: exp <- NOT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0053 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 307 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::NOT,exp); 
#line 1279 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 54: exp <- array_element_lvalue:lvalue    
AstBase* SteelParser::ReductionRuleHandler0054 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayElement* lvalue = static_cast< AstArrayElement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 309 "steel.trison"
 return lvalue; 
#line 1291 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 55: exp_statement <- ';':semi    
AstBase* SteelParser::ReductionRuleHandler0055 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 314 "steel.trison"

			int line = semi->GetLine();
			std::string script = semi->GetScript(); 
			delete semi;
			return new AstExpression(line,script); 
		
#line 1308 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 56: exp_statement <- exp:exp ';'    
AstBase* SteelParser::ReductionRuleHandler0056 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 321 "steel.trison"
 return exp; 
#line 1320 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 57: int_literal <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0057 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 326 "steel.trison"
 return i; 
#line 1332 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 58: var_identifier <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0058 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 331 "steel.trison"
 return id; 
#line 1344 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 59: func_identifier <- FUNC_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0059 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 336 "steel.trison"
 return id; 
#line 1356 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 60: array_identifier <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0060 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 341 "steel.trison"
 return id; 
#line 1368 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 61: array_element_lvalue <- array_identifier:id '[' exp:exp ']'    
AstBase* SteelParser::ReductionRuleHandler0061 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 346 "steel.trison"
 return new AstArrayElement(id->GetLine(),id->GetScript(),id,exp); 
#line 1382 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 62: call <- func_identifier:id '(' ')'    
AstBase* SteelParser::ReductionRuleHandler0062 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 351 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id);
#line 1394 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 63: call <- func_identifier:id '(' param_list:params ')'    
AstBase* SteelParser::ReductionRuleHandler0063 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstParamList* params = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 353 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id,params);
#line 1408 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 64: vardecl <- VAR var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0064 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 358 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id);
#line 1420 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 65: vardecl <- VAR var_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0065 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 360 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id,exp); 
#line 1434 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 66: vardecl <- VAR array_identifier:id '[' int_literal:i ']'    
AstBase* SteelParser::ReductionRuleHandler0066 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstInteger* i = static_cast< AstInteger* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 362 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id,i); 
#line 1448 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 67: vardecl <- VAR array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0067 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 364 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id); 
#line 1460 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 68: vardecl <- VAR array_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0068 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 366 "steel.trison"

							AstArrayDeclaration *pDecl =  new AstArrayDeclaration(id->GetLine(),id->GetScript(),id);
							pDecl->assign(exp);
							return pDecl;
						 
#line 1478 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 69: param_list <- exp:exp    
AstBase* SteelParser::ReductionRuleHandler0069 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 375 "steel.trison"
 AstParamList * pList = new AstParamList ( exp->GetLine(), exp->GetScript() );
		  pList->add(exp);
		  return pList;
		
#line 1493 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 70: param_list <- param_list:list ',' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0070 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamList* list = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 380 "steel.trison"
 list->add(exp); return list;
#line 1507 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
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
    {      Token::func_definition__,  8, &SteelParser::ReductionRuleHandler0005, "rule 5: func_definition <- FUNCTION FUNC_IDENTIFIER '(' param_definition ')' '{' statement_list '}'    "},
    {      Token::func_definition__,  7, &SteelParser::ReductionRuleHandler0006, "rule 6: func_definition <- FUNCTION FUNC_IDENTIFIER '(' ')' '{' statement_list '}'    "},
    {             Token::param_id__,  1, &SteelParser::ReductionRuleHandler0007, "rule 7: param_id <- VAR_IDENTIFIER    "},
    {             Token::param_id__,  1, &SteelParser::ReductionRuleHandler0008, "rule 8: param_id <- ARRAY_IDENTIFIER    "},
    {     Token::param_definition__,  1, &SteelParser::ReductionRuleHandler0009, "rule 9: param_definition <- vardecl    "},
    {     Token::param_definition__,  3, &SteelParser::ReductionRuleHandler0010, "rule 10: param_definition <- param_definition ',' vardecl    "},
    {       Token::statement_list__,  1, &SteelParser::ReductionRuleHandler0011, "rule 11: statement_list <- statement    "},
    {       Token::statement_list__,  2, &SteelParser::ReductionRuleHandler0012, "rule 12: statement_list <- statement_list statement    "},
    {            Token::statement__,  1, &SteelParser::ReductionRuleHandler0013, "rule 13: statement <- exp_statement    "},
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
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0038, "rule 38: exp <- var_identifier '=' exp     %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0039, "rule 39: exp <- array_element_lvalue '=' exp     %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0040, "rule 40: exp <- array_identifier '=' exp     %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0041, "rule 41: exp <- exp '^' exp    %right %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0042, "rule 42: exp <- exp OR exp    %left %prec OR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0043, "rule 43: exp <- exp AND exp    %left %prec AND"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0044, "rule 44: exp <- exp EQ exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0045, "rule 45: exp <- exp NE exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0046, "rule 46: exp <- exp LT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0047, "rule 47: exp <- exp GT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0048, "rule 48: exp <- exp LTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0049, "rule 49: exp <- exp GTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0050, "rule 50: exp <- '(' exp ')'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0051, "rule 51: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0052, "rule 52: exp <- '+' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0053, "rule 53: exp <- NOT exp     %prec UNARY"},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0054, "rule 54: exp <- array_element_lvalue    "},
    {        Token::exp_statement__,  1, &SteelParser::ReductionRuleHandler0055, "rule 55: exp_statement <- ';'    "},
    {        Token::exp_statement__,  2, &SteelParser::ReductionRuleHandler0056, "rule 56: exp_statement <- exp ';'    "},
    {          Token::int_literal__,  1, &SteelParser::ReductionRuleHandler0057, "rule 57: int_literal <- INT    "},
    {       Token::var_identifier__,  1, &SteelParser::ReductionRuleHandler0058, "rule 58: var_identifier <- VAR_IDENTIFIER    "},
    {      Token::func_identifier__,  1, &SteelParser::ReductionRuleHandler0059, "rule 59: func_identifier <- FUNC_IDENTIFIER    "},
    {     Token::array_identifier__,  1, &SteelParser::ReductionRuleHandler0060, "rule 60: array_identifier <- ARRAY_IDENTIFIER    "},
    { Token::array_element_lvalue__,  4, &SteelParser::ReductionRuleHandler0061, "rule 61: array_element_lvalue <- array_identifier '[' exp ']'    "},
    {                 Token::call__,  3, &SteelParser::ReductionRuleHandler0062, "rule 62: call <- func_identifier '(' ')'    "},
    {                 Token::call__,  4, &SteelParser::ReductionRuleHandler0063, "rule 63: call <- func_identifier '(' param_list ')'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0064, "rule 64: vardecl <- VAR var_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0065, "rule 65: vardecl <- VAR var_identifier '=' exp    "},
    {              Token::vardecl__,  5, &SteelParser::ReductionRuleHandler0066, "rule 66: vardecl <- VAR array_identifier '[' int_literal ']'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0067, "rule 67: vardecl <- VAR array_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0068, "rule 68: vardecl <- VAR array_identifier '=' exp    "},
    {           Token::param_list__,  1, &SteelParser::ReductionRuleHandler0069, "rule 69: param_list <- exp    "},
    {           Token::param_list__,  3, &SteelParser::ReductionRuleHandler0070, "rule 70: param_list <- param_list ',' exp    "},

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
    {   1,   20,    0,   21,   13}, // state    0
    {   0,    0,   34,    0,    0}, // state    1
    {  35,   10,    0,   45,    6}, // state    2
    {  51,   20,    0,   71,   10}, // state    3
    {  81,   10,    0,   91,    6}, // state    4
    {  97,   10,    0,  107,    6}, // state    5
    { 113,   10,    0,  123,    6}, // state    6
    { 129,    1,    0,    0,    0}, // state    7
    { 130,    1,    0,    0,    0}, // state    8
    { 131,    1,    0,    0,    0}, // state    9
    { 132,   11,    0,  143,    6}, // state   10
    { 149,    1,    0,    0,    0}, // state   11
    { 150,    1,    0,    0,    0}, // state   12
    {   0,    0,  151,    0,    0}, // state   13
    {   0,    0,  152,    0,    0}, // state   14
    {   0,    0,  153,    0,    0}, // state   15
    { 154,    1,    0,    0,    0}, // state   16
    { 155,    2,    0,  157,    2}, // state   17
    {   0,    0,  159,    0,    0}, // state   18
    {   0,    0,  160,    0,    0}, // state   19
    {   0,    0,  161,    0,    0}, // state   20
    { 162,    1,    0,    0,    0}, // state   21
    { 163,    1,  164,  165,    1}, // state   22
    {   0,    0,  166,    0,    0}, // state   23
    { 167,   19,  186,  187,    9}, // state   24
    {   0,    0,  196,    0,    0}, // state   25
    { 197,   16,    0,    0,    0}, // state   26
    {   0,    0,  213,    0,    0}, // state   27
    { 214,    1,  215,    0,    0}, // state   28
    { 216,    1,    0,    0,    0}, // state   29
    { 217,    2,  219,    0,    0}, // state   30
    { 220,    1,  221,    0,    0}, // state   31
    {   0,    0,  222,    0,    0}, // state   32
    { 223,    1,    0,    0,    0}, // state   33
    { 224,   16,    0,    0,    0}, // state   34
    {   0,    0,  240,    0,    0}, // state   35
    { 241,   20,    0,  261,    9}, // state   36
    { 270,    2,  272,    0,    0}, // state   37
    { 273,    2,  275,    0,    0}, // state   38
    { 276,    2,  278,    0,    0}, // state   39
    { 279,   10,    0,  289,    6}, // state   40
    {   0,    0,  295,    0,    0}, // state   41
    {   0,    0,  296,    0,    0}, // state   42
    {   0,    0,  297,    0,    0}, // state   43
    { 298,   16,    0,    0,    0}, // state   44
    { 314,   10,    0,  324,    6}, // state   45
    { 330,    1,    0,    0,    0}, // state   46
    { 331,   11,    0,  342,    7}, // state   47
    { 349,    1,  350,    0,    0}, // state   48
    { 351,    2,  353,    0,    0}, // state   49
    {   0,    0,  354,    0,    0}, // state   50
    {   0,    0,  355,    0,    0}, // state   51
    {   0,    0,  356,    0,    0}, // state   52
    {   0,    0,  357,    0,    0}, // state   53
    { 358,   10,    0,  368,    6}, // state   54
    { 374,   10,    0,  384,    6}, // state   55
    { 390,   10,    0,  400,    6}, // state   56
    { 406,   10,    0,  416,    6}, // state   57
    { 422,   10,    0,  432,    6}, // state   58
    { 438,   10,    0,  448,    6}, // state   59
    { 454,   10,    0,  464,    6}, // state   60
    { 470,   10,    0,  480,    6}, // state   61
    { 486,   10,    0,  496,    6}, // state   62
    { 502,   10,    0,  512,    6}, // state   63
    { 518,   10,    0,  528,    6}, // state   64
    { 534,   10,    0,  544,    6}, // state   65
    { 550,   10,    0,  560,    6}, // state   66
    { 566,   10,    0,  576,    6}, // state   67
    { 582,   10,    0,  592,    6}, // state   68
    { 598,   10,    0,  608,    6}, // state   69
    { 614,   11,    0,  625,    7}, // state   70
    { 632,   10,    0,  642,    6}, // state   71
    { 648,   10,    0,  658,    6}, // state   72
    { 664,   10,    0,  674,    6}, // state   73
    {   0,    0,  680,    0,    0}, // state   74
    {   0,    0,  681,    0,    0}, // state   75
    {   0,    0,  682,    0,    0}, // state   76
    { 683,   16,    0,    0,    0}, // state   77
    {   0,    0,  699,    0,    0}, // state   78
    { 700,   16,    0,    0,    0}, // state   79
    { 716,    2,    0,  718,    2}, // state   80
    { 720,   11,    0,  731,    7}, // state   81
    { 738,   10,    0,  748,    6}, // state   82
    { 754,   10,    0,  764,    6}, // state   83
    { 770,    1,    0,  771,    1}, // state   84
    { 772,    5,  777,    0,    0}, // state   85
    { 778,    5,  783,    0,    0}, // state   86
    { 784,    2,  786,    0,    0}, // state   87
    { 787,    2,  789,    0,    0}, // state   88
    { 790,    1,  791,    0,    0}, // state   89
    { 792,    2,  794,    0,    0}, // state   90
    { 795,    1,  796,    0,    0}, // state   91
    { 797,    7,  804,    0,    0}, // state   92
    { 805,    7,  812,    0,    0}, // state   93
    { 813,   11,  824,    0,    0}, // state   94
    { 825,   11,  836,    0,    0}, // state   95
    { 837,    7,  844,    0,    0}, // state   96
    { 845,    7,  852,    0,    0}, // state   97
    { 853,   13,  866,    0,    0}, // state   98
    { 867,   14,  881,    0,    0}, // state   99
    { 882,   15,  897,    0,    0}, // state  100
    {   0,    0,  898,    0,    0}, // state  101
    { 899,   15,  914,    0,    0}, // state  102
    { 915,    2,    0,    0,    0}, // state  103
    { 917,   15,  932,    0,    0}, // state  104
    { 933,   16,    0,    0,    0}, // state  105
    { 949,   15,  964,    0,    0}, // state  106
    { 965,   19,    0,  984,    9}, // state  107
    { 993,   19,    0, 1012,    9}, // state  108
    {1021,    1,    0,    0,    0}, // state  109
    {1022,    2,    0,    0,    0}, // state  110
    {   0,    0, 1024,    0,    0}, // state  111
    {1025,   11,    0, 1036,    6}, // state  112
    {1042,   15, 1057,    0,    0}, // state  113
    {1058,   15, 1073,    0,    0}, // state  114
    {   0,    0, 1074,    0,    0}, // state  115
    {1075,    1,    0,    0,    0}, // state  116
    {   0,    0, 1076,    0,    0}, // state  117
    {1077,   10,    0, 1087,    6}, // state  118
    {   0,    0, 1093,    0,    0}, // state  119
    {   0,    0, 1094,    0,    0}, // state  120
    {1095,    1, 1096,    0,    0}, // state  121
    {1097,   19,    0, 1116,   10}, // state  122
    {1126,    1,    0,    0,    0}, // state  123
    {1127,    1,    0, 1128,    1}, // state  124
    {1129,   19,    0, 1148,    9}, // state  125
    {1157,   16,    0,    0,    0}, // state  126
    {   0,    0, 1173,    0,    0}, // state  127
    {1174,   15, 1189,    0,    0}, // state  128
    {1190,   19,    0, 1209,    9}, // state  129
    {1218,   20,    0, 1238,    9}, // state  130
    {1247,   19,    0, 1266,   10}, // state  131
    {   0,    0, 1276,    0,    0}, // state  132
    {   0,    0, 1277,    0,    0}, // state  133
    {1278,   19,    0, 1297,    9}, // state  134
    {   0,    0, 1306,    0,    0}, // state  135
    {   0,    0, 1307,    0,    0}, // state  136
    {1308,   20,    0, 1328,    9}, // state  137
    {   0,    0, 1337,    0,    0}, // state  138
    {   0,    0, 1338,    0,    0}  // state  139

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
    // nonterminal transitions
    {                 Token::root__, {                  TA_PUSH_STATE,   21}},
    { Token::func_definition_list__, {                  TA_PUSH_STATE,   22}},
    {      Token::func_definition__, {                  TA_PUSH_STATE,   23}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   24}},
    {            Token::statement__, {                  TA_PUSH_STATE,   25}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   34}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {       Token::statement_list__, {                  TA_PUSH_STATE,   36}},
    {            Token::statement__, {                  TA_PUSH_STATE,   25}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   37}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   38}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   39}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   44}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state   22
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {               Token::FUNCTION, {        TA_SHIFT_AND_PUSH_STATE,   12}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    2}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   51}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,   52}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   69}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   30}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   70}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   72}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   73}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   54}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   26}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   74}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,   52}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   51}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   53}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   77}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   41
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   42
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   25}},

// ///////////////////////////////////////////////////////////////////////////
// state   43
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state   44
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   78}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   79}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   46
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   80}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   81}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   48
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   82}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   64}},

// ///////////////////////////////////////////////////////////////////////////
// state   49
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   83}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   84}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   67}},

// ///////////////////////////////////////////////////////////////////////////
// state   50
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state   51
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    4}},

// ///////////////////////////////////////////////////////////////////////////
// state   52
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   53
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   56}},

// ///////////////////////////////////////////////////////////////////////////
// state   54
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   85}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   86}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   87}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   88}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   89}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   90}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   91}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   92}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   93}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   94}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   95}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   96}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   97}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   98}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   99}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  100}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   70
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  101}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  102}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {           Token::param_list__, {                  TA_PUSH_STATE,  103}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  104}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  105}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  106}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   74
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state   75
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state   76
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state   77
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  107}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state   78
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state   79
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  108}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state   80
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  109}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,  110}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,  111}},

// ///////////////////////////////////////////////////////////////////////////
// state   81
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,  112}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   82
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  113}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  114}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   84
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,  115}},
    // nonterminal transitions
    {          Token::int_literal__, {                  TA_PUSH_STATE,  116}},

// ///////////////////////////////////////////////////////////////////////////
// state   85
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   86
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   87
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   88
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   89
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state   90
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   91
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state   92
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   93
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state   94
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state   95
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state   96
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   97
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state   98
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state   99
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state  100
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state  101
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   62}},

// ///////////////////////////////////////////////////////////////////////////
// state  102
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   69}},

// ///////////////////////////////////////////////////////////////////////////
// state  103
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  117}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  118}},

// ///////////////////////////////////////////////////////////////////////////
// state  104
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state  105
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  119}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state  106
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   39}},

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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  120}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  108
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  121}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  109
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  122}},

// ///////////////////////////////////////////////////////////////////////////
// state  110
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  123}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  124}},

// ///////////////////////////////////////////////////////////////////////////
// state  111
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state  112
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  125}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  126}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  113
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   65}},

// ///////////////////////////////////////////////////////////////////////////
// state  114
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state  115
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   57}},

// ///////////////////////////////////////////////////////////////////////////
// state  116
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  127}},

// ///////////////////////////////////////////////////////////////////////////
// state  117
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   63}},

// ///////////////////////////////////////////////////////////////////////////
// state  118
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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  128}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  119
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state  120
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state  121
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::ELSE, {        TA_SHIFT_AND_PUSH_STATE,  129}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state  122
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {       Token::statement_list__, {                  TA_PUSH_STATE,  130}},
    {            Token::statement__, {                  TA_PUSH_STATE,   25}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  123
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  131}},

// ///////////////////////////////////////////////////////////////////////////
// state  124
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {              Token::vardecl__, {                  TA_PUSH_STATE,  132}},

// ///////////////////////////////////////////////////////////////////////////
// state  125
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  133}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  126
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  134}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state  127
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state  128
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   70}},

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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  135}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  130
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  136}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,   52}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  131
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {       Token::statement_list__, {                  TA_PUSH_STATE,  137}},
    {            Token::statement__, {                  TA_PUSH_STATE,   25}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  132
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state  133
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state  134
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
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,  138}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  135
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state  136
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state  137
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  139}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,    4}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,    5}},
    {                    Token::NOT, {        TA_SHIFT_AND_PUSH_STATE,    6}},
    {                  Token::WHILE, {        TA_SHIFT_AND_PUSH_STATE,    7}},
    {                  Token::BREAK, {        TA_SHIFT_AND_PUSH_STATE,    8}},
    {               Token::CONTINUE, {        TA_SHIFT_AND_PUSH_STATE,    9}},
    {                 Token::RETURN, {        TA_SHIFT_AND_PUSH_STATE,   10}},
    {                     Token::IF, {        TA_SHIFT_AND_PUSH_STATE,   11}},
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   13}},
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    {                    Token::FOR, {        TA_SHIFT_AND_PUSH_STATE,   16}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    {                    Token::INT, {        TA_SHIFT_AND_PUSH_STATE,   18}},
    {                  Token::FLOAT, {        TA_SHIFT_AND_PUSH_STATE,   19}},
    {                 Token::STRING, {        TA_SHIFT_AND_PUSH_STATE,   20}},
    // nonterminal transitions
    {            Token::statement__, {                  TA_PUSH_STATE,   52}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   26}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   27}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   28}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   29}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   30}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   31}},
    {                 Token::call__, {                  TA_PUSH_STATE,   32}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state  138
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state  139
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    5}}

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

#line 3906 "SteelParser.cpp"

