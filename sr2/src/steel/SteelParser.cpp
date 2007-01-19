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

#line 98 "steel.trison"

				AstScript * pScript =   new AstScript(
							list->GetLine(),
							list->GetScript());
			        pScript->SetList(list);
				return pScript;
			
#line 513 "SteelParser.cpp"
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

#line 111 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									params,
									stmts);
				
#line 541 "SteelParser.cpp"
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

#line 122 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									NULL,
									stmts);
				
#line 567 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 4: param_id <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0004 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 136 "steel.trison"
 return id; 
#line 579 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 5: param_id <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0005 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 138 "steel.trison"
 return id; 
#line 591 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 6: param_definition <- vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0006 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 144 "steel.trison"

				AstParamDefinitionList * pList = new AstParamDefinitionList(decl->GetLine(),decl->GetScript());
				pList->add(static_cast<AstDeclaration*>(decl));
				return pList;		
			
#line 607 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 7: param_definition <- param_definition:list ',' vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0007 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamDefinitionList* list = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 151 "steel.trison"

				list->add(static_cast<AstDeclaration*>(decl));
				return list;
			
#line 624 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 8: statement_list <- statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0008 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 159 "steel.trison"

				AstStatementList *pList = new AstStatementList( stmt->GetLine(),
										stmt->GetScript());
				pList->add(stmt);
				return pList;
			
#line 641 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 9: statement_list <- statement_list:list statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0009 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 167 "steel.trison"

					list->add( stmt ); 
					return list;
				
#line 658 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 10: statement <- exp_statement:exp    
AstBase* SteelParser::ReductionRuleHandler0010 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 175 "steel.trison"
 return new AstExpressionStatement(exp->GetLine(),exp->GetScript(), exp); 
#line 670 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 11: statement <- func_definition:func    
AstBase* SteelParser::ReductionRuleHandler0011 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinition* func = static_cast< AstFunctionDefinition* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 177 "steel.trison"
 return func; 
#line 682 "SteelParser.cpp"
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

#line 179 "steel.trison"
 delete b1; delete b2; return list; 
#line 698 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 13: statement <- '{':b1 '}':b2    
AstBase* SteelParser::ReductionRuleHandler0013 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 181 "steel.trison"

			 int line = b1->GetLine();
			 std::string script = b1->GetScript();
			 delete b1;
			 delete b2;
			 return new AstStatement(line,script);
			
#line 718 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 14: statement <- vardecl:vardecl ';':semi    
AstBase* SteelParser::ReductionRuleHandler0014 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* vardecl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 189 "steel.trison"
 delete semi; return vardecl; 
#line 732 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 15: statement <- WHILE '(' exp:exp ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0015 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 191 "steel.trison"
 return new AstWhileStatement(exp->GetLine(), exp->GetScript(),exp,stmt); 
#line 746 "SteelParser.cpp"
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

#line 193 "steel.trison"
 return new AstIfStatement(exp->GetLine(), exp->GetScript(),exp,stmt,elses);
#line 762 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 17: statement <- IF '(' exp:exp ')' statement:stmt     %prec NON_ELSE
AstBase* SteelParser::ReductionRuleHandler0017 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 195 "steel.trison"
 return new AstIfStatement(exp->GetLine(),exp->GetScript(),exp,stmt); 
#line 776 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 18: statement <- RETURN exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0018 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 197 "steel.trison"
 delete semi; return new AstReturnStatement(exp->GetLine(),exp->GetScript(),exp);
#line 790 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 19: statement <- RETURN ';':semi    
AstBase* SteelParser::ReductionRuleHandler0019 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 200 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstReturnStatement(line,script);
			
#line 807 "SteelParser.cpp"
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

#line 208 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition, 
						new AstExpression (start->GetLine(),start->GetScript()), stmt); 
			
#line 826 "SteelParser.cpp"
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

#line 214 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition,iteration,stmt);
			
#line 846 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 22: statement <- BREAK ';':semi    
AstBase* SteelParser::ReductionRuleHandler0022 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 219 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstBreakStatement(line,script); 
			
#line 863 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 23: statement <- CONTINUE ';':semi    
AstBase* SteelParser::ReductionRuleHandler0023 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 227 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstContinueStatement(line,script); 
			
#line 880 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 24: exp <- call:call    
AstBase* SteelParser::ReductionRuleHandler0024 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstCallExpression* call = static_cast< AstCallExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 238 "steel.trison"
 return call; 
#line 892 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 25: exp <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0025 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 240 "steel.trison"
 return i;
#line 904 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 26: exp <- FLOAT:f    
AstBase* SteelParser::ReductionRuleHandler0026 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* f = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 242 "steel.trison"
 return f; 
#line 916 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 27: exp <- STRING:s    
AstBase* SteelParser::ReductionRuleHandler0027 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* s = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 244 "steel.trison"
 return s; 
#line 928 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 28: exp <- var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0028 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 246 "steel.trison"
 return id; 
#line 940 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 29: exp <- array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0029 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 248 "steel.trison"
 return id; 
#line 952 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 30: exp <- exp:a '+' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0030 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 250 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::ADD,a,b); 
#line 966 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 31: exp <- exp:a '-' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0031 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 252 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::SUB,a,b); 
#line 980 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 32: exp <- exp:a '*' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0032 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 254 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MULT,a,b); 
#line 994 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 33: exp <- exp:a '/' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0033 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 256 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::DIV,a,b); 
#line 1008 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 34: exp <- exp:a '%' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0034 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 258 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MOD,a,b); 
#line 1022 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 35: exp <- exp:a D exp:b    %left %prec POW
AstBase* SteelParser::ReductionRuleHandler0035 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 260 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::D,a,b); 
#line 1036 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 36: exp <- var_identifier:id '=' exp:exp     %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0036 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 262 "steel.trison"
 return new AstVarAssignmentExpression(id->GetLine(),id->GetScript(),id,exp); 
#line 1050 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 37: exp <- array_element_lvalue:id '=' exp:exp     %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0037 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayElement* id = static_cast< AstArrayElement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 264 "steel.trison"
 return new AstArrayElementAssignmentExpression(id->GetLine(),id->GetScript(),id,exp); 
#line 1064 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 38: exp <- array_identifier:id '=' exp:exp     %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0038 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 266 "steel.trison"
 return new AstArrayAssignmentExpression(id->GetLine(),id->GetScript(),id,exp); 
#line 1078 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 39: exp <- exp:a '^' exp:b    %right %prec POW
AstBase* SteelParser::ReductionRuleHandler0039 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 268 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::POW,a,b); 
#line 1092 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 40: exp <- exp:a OR exp:b    %left %prec OR
AstBase* SteelParser::ReductionRuleHandler0040 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 270 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::OR,a,b); 
#line 1106 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 41: exp <- exp:a AND exp:b    %left %prec AND
AstBase* SteelParser::ReductionRuleHandler0041 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 272 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::AND,a,b); 
#line 1120 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 42: exp <- exp:a EQ exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0042 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 274 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::EQ,a,b); 
#line 1134 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 43: exp <- exp:a NE exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0043 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 276 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::NE,a,b); 
#line 1148 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 44: exp <- exp:a LT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0044 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 278 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LT,a,b); 
#line 1162 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 45: exp <- exp:a GT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0045 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 280 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GT,a,b); 
#line 1176 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 46: exp <- exp:a LTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0046 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 282 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LTE,a,b); 
#line 1190 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 47: exp <- exp:a GTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0047 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 284 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GTE,a,b); 
#line 1204 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 48: exp <- '(' exp:exp ')'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0048 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 286 "steel.trison"
 return exp; 
#line 1216 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 49: exp <- '-' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0049 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 288 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::MINUS,exp); 
#line 1228 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 50: exp <- '+' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0050 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 290 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::PLUS,exp); 
#line 1240 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 51: exp <- NOT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0051 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 292 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::NOT,exp); 
#line 1252 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 52: exp <- array_element_lvalue:lvalue    
AstBase* SteelParser::ReductionRuleHandler0052 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayElement* lvalue = static_cast< AstArrayElement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 294 "steel.trison"
 return lvalue; 
#line 1264 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 53: exp_statement <- ';':semi    
AstBase* SteelParser::ReductionRuleHandler0053 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 299 "steel.trison"

			int line = semi->GetLine();
			std::string script = semi->GetScript(); 
			delete semi;
			return new AstExpression(line,script); 
		
#line 1281 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 54: exp_statement <- exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0054 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 306 "steel.trison"
 delete semi;  return exp; 
#line 1295 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 55: int_literal <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0055 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 311 "steel.trison"
 return i; 
#line 1307 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 56: var_identifier <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0056 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 316 "steel.trison"
 return id; 
#line 1319 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 57: func_identifier <- FUNC_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0057 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 321 "steel.trison"
 return id; 
#line 1331 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 58: array_identifier <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0058 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 326 "steel.trison"
 return id; 
#line 1343 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 59: array_element_lvalue <- array_identifier:id '[' exp:exp ']'    
AstBase* SteelParser::ReductionRuleHandler0059 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 331 "steel.trison"
 return new AstArrayElement(id->GetLine(),id->GetScript(),id,exp); 
#line 1357 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 60: call <- func_identifier:id '(' ')'    
AstBase* SteelParser::ReductionRuleHandler0060 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 336 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id);
#line 1369 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 61: call <- func_identifier:id '(' param_list:params ')'    
AstBase* SteelParser::ReductionRuleHandler0061 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstParamList* params = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 338 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id,params);
#line 1383 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 62: vardecl <- VAR var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0062 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 343 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id);
#line 1395 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 63: vardecl <- VAR var_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0063 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 345 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id,exp); 
#line 1409 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 64: vardecl <- VAR array_identifier:id '[' exp:i ']'    
AstBase* SteelParser::ReductionRuleHandler0064 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* i = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 347 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id,i); 
#line 1423 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 65: vardecl <- VAR array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0065 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 349 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id); 
#line 1435 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 66: vardecl <- VAR array_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0066 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 351 "steel.trison"

							AstArrayDeclaration *pDecl =  new AstArrayDeclaration(id->GetLine(),id->GetScript(),id);
							pDecl->assign(exp);
							return pDecl;
						 
#line 1453 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 67: param_list <- exp:exp    
AstBase* SteelParser::ReductionRuleHandler0067 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 360 "steel.trison"
 AstParamList * pList = new AstParamList ( exp->GetLine(), exp->GetScript() );
		  pList->add(exp);
		  return pList;
		
#line 1468 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 68: param_list <- param_list:list ',' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0068 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamList* list = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 365 "steel.trison"
 list->add(exp); return list;
#line 1482 "SteelParser.cpp"
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
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0036, "rule 36: exp <- var_identifier '=' exp     %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0037, "rule 37: exp <- array_element_lvalue '=' exp     %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0038, "rule 38: exp <- array_identifier '=' exp     %prec ASSIGNMENT"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0039, "rule 39: exp <- exp '^' exp    %right %prec POW"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0040, "rule 40: exp <- exp OR exp    %left %prec OR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0041, "rule 41: exp <- exp AND exp    %left %prec AND"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0042, "rule 42: exp <- exp EQ exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0043, "rule 43: exp <- exp NE exp    %left %prec EQ_NE"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0044, "rule 44: exp <- exp LT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0045, "rule 45: exp <- exp GT exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0046, "rule 46: exp <- exp LTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0047, "rule 47: exp <- exp GTE exp    %left %prec COMPARATOR"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0048, "rule 48: exp <- '(' exp ')'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0049, "rule 49: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0050, "rule 50: exp <- '+' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0051, "rule 51: exp <- NOT exp     %prec UNARY"},
    {                  Token::exp__,  1, &SteelParser::ReductionRuleHandler0052, "rule 52: exp <- array_element_lvalue    "},
    {        Token::exp_statement__,  1, &SteelParser::ReductionRuleHandler0053, "rule 53: exp_statement <- ';'    "},
    {        Token::exp_statement__,  2, &SteelParser::ReductionRuleHandler0054, "rule 54: exp_statement <- exp ';'    "},
    {          Token::int_literal__,  1, &SteelParser::ReductionRuleHandler0055, "rule 55: int_literal <- INT    "},
    {       Token::var_identifier__,  1, &SteelParser::ReductionRuleHandler0056, "rule 56: var_identifier <- VAR_IDENTIFIER    "},
    {      Token::func_identifier__,  1, &SteelParser::ReductionRuleHandler0057, "rule 57: func_identifier <- FUNC_IDENTIFIER    "},
    {     Token::array_identifier__,  1, &SteelParser::ReductionRuleHandler0058, "rule 58: array_identifier <- ARRAY_IDENTIFIER    "},
    { Token::array_element_lvalue__,  4, &SteelParser::ReductionRuleHandler0059, "rule 59: array_element_lvalue <- array_identifier '[' exp ']'    "},
    {                 Token::call__,  3, &SteelParser::ReductionRuleHandler0060, "rule 60: call <- func_identifier '(' ')'    "},
    {                 Token::call__,  4, &SteelParser::ReductionRuleHandler0061, "rule 61: call <- func_identifier '(' param_list ')'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0062, "rule 62: vardecl <- VAR var_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0063, "rule 63: vardecl <- VAR var_identifier '=' exp    "},
    {              Token::vardecl__,  5, &SteelParser::ReductionRuleHandler0064, "rule 64: vardecl <- VAR array_identifier '[' exp ']'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0065, "rule 65: vardecl <- VAR array_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0066, "rule 66: vardecl <- VAR array_identifier '=' exp    "},
    {           Token::param_list__,  1, &SteelParser::ReductionRuleHandler0067, "rule 67: param_list <- exp    "},
    {           Token::param_list__,  3, &SteelParser::ReductionRuleHandler0068, "rule 68: param_list <- param_list ',' exp    "},

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
    {   1,   20,    0,   21,   12}, // state    0
    {   0,    0,   33,    0,    0}, // state    1
    {  34,   10,    0,   44,    6}, // state    2
    {  50,   21,    0,   71,   11}, // state    3
    {  82,   10,    0,   92,    6}, // state    4
    {  98,   10,    0,  108,    6}, // state    5
    { 114,   10,    0,  124,    6}, // state    6
    { 130,    1,    0,    0,    0}, // state    7
    { 131,    1,    0,    0,    0}, // state    8
    { 132,    1,    0,    0,    0}, // state    9
    { 133,   11,    0,  144,    6}, // state   10
    { 150,    1,    0,    0,    0}, // state   11
    { 151,    1,    0,    0,    0}, // state   12
    {   0,    0,  152,    0,    0}, // state   13
    {   0,    0,  153,    0,    0}, // state   14
    {   0,    0,  154,    0,    0}, // state   15
    { 155,    1,    0,    0,    0}, // state   16
    { 156,    2,    0,  158,    2}, // state   17
    {   0,    0,  160,    0,    0}, // state   18
    {   0,    0,  161,    0,    0}, // state   19
    {   0,    0,  162,    0,    0}, // state   20
    { 163,    1,    0,    0,    0}, // state   21
    {   0,    0,  164,    0,    0}, // state   22
    { 165,   20,  185,  186,   10}, // state   23
    {   0,    0,  196,    0,    0}, // state   24
    { 197,   16,    0,    0,    0}, // state   25
    {   0,    0,  213,    0,    0}, // state   26
    { 214,    1,  215,    0,    0}, // state   27
    { 216,    1,    0,    0,    0}, // state   28
    { 217,    2,  219,    0,    0}, // state   29
    { 220,    1,  221,    0,    0}, // state   30
    {   0,    0,  222,    0,    0}, // state   31
    { 223,    1,    0,    0,    0}, // state   32
    { 224,   16,    0,    0,    0}, // state   33
    {   0,    0,  240,    0,    0}, // state   34
    { 241,   21,    0,  262,   10}, // state   35
    { 272,    2,  274,    0,    0}, // state   36
    { 275,    2,  277,    0,    0}, // state   37
    { 278,    2,  280,    0,    0}, // state   38
    { 281,   10,    0,  291,    6}, // state   39
    {   0,    0,  297,    0,    0}, // state   40
    {   0,    0,  298,    0,    0}, // state   41
    {   0,    0,  299,    0,    0}, // state   42
    { 300,   16,    0,    0,    0}, // state   43
    { 316,   10,    0,  326,    6}, // state   44
    { 332,    1,    0,    0,    0}, // state   45
    { 333,   11,    0,  344,    7}, // state   46
    { 351,    1,  352,    0,    0}, // state   47
    { 353,    2,  355,    0,    0}, // state   48
    {   0,    0,  356,    0,    0}, // state   49
    {   0,    0,  357,    0,    0}, // state   50
    {   0,    0,  358,    0,    0}, // state   51
    { 359,   10,    0,  369,    6}, // state   52
    { 375,   10,    0,  385,    6}, // state   53
    { 391,   10,    0,  401,    6}, // state   54
    { 407,   10,    0,  417,    6}, // state   55
    { 423,   10,    0,  433,    6}, // state   56
    { 439,   10,    0,  449,    6}, // state   57
    { 455,   10,    0,  465,    6}, // state   58
    { 471,   10,    0,  481,    6}, // state   59
    { 487,   10,    0,  497,    6}, // state   60
    { 503,   10,    0,  513,    6}, // state   61
    { 519,   10,    0,  529,    6}, // state   62
    { 535,   10,    0,  545,    6}, // state   63
    { 551,   10,    0,  561,    6}, // state   64
    { 567,   10,    0,  577,    6}, // state   65
    { 583,   10,    0,  593,    6}, // state   66
    { 599,   10,    0,  609,    6}, // state   67
    { 615,   11,    0,  626,    7}, // state   68
    { 633,   10,    0,  643,    6}, // state   69
    { 649,   10,    0,  659,    6}, // state   70
    { 665,   10,    0,  675,    6}, // state   71
    {   0,    0,  681,    0,    0}, // state   72
    {   0,    0,  682,    0,    0}, // state   73
    {   0,    0,  683,    0,    0}, // state   74
    { 684,   16,    0,    0,    0}, // state   75
    {   0,    0,  700,    0,    0}, // state   76
    { 701,   16,    0,    0,    0}, // state   77
    { 717,    2,    0,  719,    2}, // state   78
    { 721,   11,    0,  732,    7}, // state   79
    { 739,   10,    0,  749,    6}, // state   80
    { 755,   10,    0,  765,    6}, // state   81
    { 771,   10,    0,  781,    6}, // state   82
    { 787,    5,  792,    0,    0}, // state   83
    { 793,    5,  798,    0,    0}, // state   84
    { 799,    2,  801,    0,    0}, // state   85
    { 802,    2,  804,    0,    0}, // state   86
    { 805,    1,  806,    0,    0}, // state   87
    { 807,    2,  809,    0,    0}, // state   88
    { 810,    1,  811,    0,    0}, // state   89
    { 812,    7,  819,    0,    0}, // state   90
    { 820,    7,  827,    0,    0}, // state   91
    { 828,   11,  839,    0,    0}, // state   92
    { 840,   11,  851,    0,    0}, // state   93
    { 852,    7,  859,    0,    0}, // state   94
    { 860,    7,  867,    0,    0}, // state   95
    { 868,   13,  881,    0,    0}, // state   96
    { 882,   14,  896,    0,    0}, // state   97
    { 897,   15,  912,    0,    0}, // state   98
    {   0,    0,  913,    0,    0}, // state   99
    { 914,   15,  929,    0,    0}, // state  100
    { 930,    2,    0,    0,    0}, // state  101
    { 932,   15,  947,    0,    0}, // state  102
    { 948,   16,    0,    0,    0}, // state  103
    { 964,   15,  979,    0,    0}, // state  104
    { 980,   20,    0, 1000,   10}, // state  105
    {1010,   20,    0, 1030,   10}, // state  106
    {1040,    1,    0,    0,    0}, // state  107
    {1041,    2,    0,    0,    0}, // state  108
    {   0,    0, 1043,    0,    0}, // state  109
    {1044,   11,    0, 1055,    6}, // state  110
    {1061,   15, 1076,    0,    0}, // state  111
    {1077,   15, 1092,    0,    0}, // state  112
    {1093,   16,    0,    0,    0}, // state  113
    {   0,    0, 1109,    0,    0}, // state  114
    {1110,   10,    0, 1120,    6}, // state  115
    {   0,    0, 1126,    0,    0}, // state  116
    {   0,    0, 1127,    0,    0}, // state  117
    {1128,    1, 1129,    0,    0}, // state  118
    {1130,   20,    0, 1150,   11}, // state  119
    {1161,    1,    0,    0,    0}, // state  120
    {1162,    1,    0, 1163,    1}, // state  121
    {1164,   20,    0, 1184,   10}, // state  122
    {1194,   16,    0,    0,    0}, // state  123
    {   0,    0, 1210,    0,    0}, // state  124
    {1211,   15, 1226,    0,    0}, // state  125
    {1227,   20,    0, 1247,   10}, // state  126
    {1257,   21,    0, 1278,   10}, // state  127
    {1288,   20,    0, 1308,   11}, // state  128
    {   0,    0, 1319,    0,    0}, // state  129
    {   0,    0, 1320,    0,    0}, // state  130
    {1321,   20,    0, 1341,   10}, // state  131
    {   0,    0, 1351,    0,    0}, // state  132
    {   0,    0, 1352,    0,    0}, // state  133
    {1353,   21,    0, 1374,   10}, // state  134
    {   0,    0, 1384,    0,    0}, // state  135
    {   0,    0, 1385,    0,    0}  // state  136

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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   23}},
    {            Token::statement__, {                  TA_PUSH_STATE,   24}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   53}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   33}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   34}},
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   35}},
    {            Token::statement__, {                  TA_PUSH_STATE,   24}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   36}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   37}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   38}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   42}},
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   43}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   57}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   56}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   58}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    // nonterminal transitions
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   47}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   48}},

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
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   22
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   23
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
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,   50}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   24
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   51}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   67}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   70}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   71}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   72}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   74}},
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,   50}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   51}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   75}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   40
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state   41
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state   42
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   43
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   44
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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   45
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   46
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   79}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   47
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   80}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   62}},

// ///////////////////////////////////////////////////////////////////////////
// state   48
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   81}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   82}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   65}},

// ///////////////////////////////////////////////////////////////////////////
// state   49
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state   50
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state   51
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   54}},

// ///////////////////////////////////////////////////////////////////////////
// state   52
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   83}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   53
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   84}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   68
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   99}},
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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {           Token::param_list__, {                  TA_PUSH_STATE,  101}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  102}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  103}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   72
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state   73
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state   74
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   75
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  105}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   76
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   77
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  106}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   78
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  107}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,  108}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,  109}},

// ///////////////////////////////////////////////////////////////////////////
// state   79
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,  110}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,  111}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   81
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  112}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   83
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   84
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   30}},

// ///////////////////////////////////////////////////////////////////////////
// state   85
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   86
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   87
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state   88
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   89
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   90
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state   91
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state   92
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state   93
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state   94
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   95
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state   96
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state   97
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state   98
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   99
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   60}},

// ///////////////////////////////////////////////////////////////////////////
// state  100
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   67}},

// ///////////////////////////////////////////////////////////////////////////
// state  101
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  114}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  115}},

// ///////////////////////////////////////////////////////////////////////////
// state  102
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state  103
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  116}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state  104
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state  105
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,  117}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,  118}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  107
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  119}},

// ///////////////////////////////////////////////////////////////////////////
// state  108
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  120}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  121}},

// ///////////////////////////////////////////////////////////////////////////
// state  109
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state  110
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  122}},
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  123}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state  111
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   63}},

// ///////////////////////////////////////////////////////////////////////////
// state  112
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state  113
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  124}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state  114
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state  115
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  125}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state  116
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   59}},

// ///////////////////////////////////////////////////////////////////////////
// state  117
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state  118
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::ELSE, {        TA_SHIFT_AND_PUSH_STATE,  126}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

// ///////////////////////////////////////////////////////////////////////////
// state  119
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  127}},
    {            Token::statement__, {                  TA_PUSH_STATE,   24}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  120
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  128}},

// ///////////////////////////////////////////////////////////////////////////
// state  121
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {              Token::vardecl__, {                  TA_PUSH_STATE,  129}},

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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,  130}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  123
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  131}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state  124
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   64}},

// ///////////////////////////////////////////////////////////////////////////
// state  125
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   52}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   53}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   54}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   55}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   56}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   57}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   66}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state  126
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,  132}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  127
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  133}},
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,   50}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  128
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  134}},
    {            Token::statement__, {                  TA_PUSH_STATE,   24}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  129
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state  130
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,  135}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  132
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state  133
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state  134
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
    {      Token::func_definition__, {                  TA_PUSH_STATE,   22}},
    {            Token::statement__, {                  TA_PUSH_STATE,   50}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   25}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   26}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   27}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   28}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   29}},
    { Token::array_element_lvalue__, {                  TA_PUSH_STATE,   30}},
    {                 Token::call__, {                  TA_PUSH_STATE,   31}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state  135
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state  136
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

#line 3906 "SteelParser.cpp"

