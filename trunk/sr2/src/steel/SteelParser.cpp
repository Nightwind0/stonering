#include "SteelParser.hpp"

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

#line 102 "steel.trison"

				AstScript * pScript =   new AstScript(
							list->GetLine(),
							list->GetScript());
			        pScript->SetList(list);
				return pScript;
			
#line 517 "SteelParser.cpp"
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

#line 115 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									params,
									stmts);
				
#line 545 "SteelParser.cpp"
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

#line 126 "steel.trison"

					delete b1;
					delete b2;
					return new AstFunctionDefinition(id->GetLine(),
									id->GetScript(),
									static_cast<AstFuncIdentifier*>(id),
									NULL,
									stmts);
				
#line 571 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 4: param_id <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0004 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 140 "steel.trison"
 return id; 
#line 583 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 5: param_id <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0005 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 142 "steel.trison"
 return id; 
#line 595 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 6: param_definition <- vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0006 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 148 "steel.trison"

				AstParamDefinitionList * pList = new AstParamDefinitionList(decl->GetLine(),decl->GetScript());
				pList->add(static_cast<AstDeclaration*>(decl));
				return pList;		
			
#line 611 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 7: param_definition <- param_definition:list ',' vardecl:decl    
AstBase* SteelParser::ReductionRuleHandler0007 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamDefinitionList* list = static_cast< AstParamDefinitionList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstDeclaration* decl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 155 "steel.trison"

				list->add(static_cast<AstDeclaration*>(decl));
				return list;
			
#line 628 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 8: statement_list <- statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0008 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 163 "steel.trison"

				AstStatementList *pList = new AstStatementList( stmt->GetLine(),
										stmt->GetScript());
				pList->add(stmt);
				return pList;
			
#line 645 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 9: statement_list <- statement_list:list statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0009 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstStatementList* list = static_cast< AstStatementList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 171 "steel.trison"

					list->add( stmt ); 
					return list;
				
#line 662 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 10: statement <- exp_statement:exp    
AstBase* SteelParser::ReductionRuleHandler0010 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 179 "steel.trison"
 return new AstExpressionStatement(exp->GetLine(),exp->GetScript(), exp); 
#line 674 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 11: statement <- func_definition:func    
AstBase* SteelParser::ReductionRuleHandler0011 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFunctionDefinition* func = static_cast< AstFunctionDefinition* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 181 "steel.trison"
 return func; 
#line 686 "SteelParser.cpp"
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

#line 183 "steel.trison"
 delete b1; delete b2; return list; 
#line 702 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 13: statement <- '{':b1 '}':b2    
AstBase* SteelParser::ReductionRuleHandler0013 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* b1 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* b2 = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 185 "steel.trison"

			 int line = b1->GetLine();
			 std::string script = b1->GetScript();
			 delete b1;
			 delete b2;
			 return new AstStatement(line,script);
			
#line 722 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 14: statement <- vardecl:vardecl ';':semi    
AstBase* SteelParser::ReductionRuleHandler0014 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstDeclaration* vardecl = static_cast< AstDeclaration* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 193 "steel.trison"
 delete semi; return vardecl; 
#line 736 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 15: statement <- WHILE '(' exp:exp ')' statement:stmt    
AstBase* SteelParser::ReductionRuleHandler0015 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 195 "steel.trison"
 return new AstWhileStatement(exp->GetLine(), exp->GetScript(),exp,stmt); 
#line 750 "SteelParser.cpp"
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

#line 197 "steel.trison"
 return new AstIfStatement(exp->GetLine(), exp->GetScript(),exp,stmt,elses);
#line 766 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 17: statement <- IF '(' exp:exp ')' statement:stmt     %prec NON_ELSE
AstBase* SteelParser::ReductionRuleHandler0017 ()
{
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);
    assert(4 < m_reduction_rule_token_count);
    AstStatement* stmt = static_cast< AstStatement* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 4]);

#line 199 "steel.trison"
 return new AstIfStatement(exp->GetLine(),exp->GetScript(),exp,stmt); 
#line 780 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 18: statement <- RETURN exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0018 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(2 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 201 "steel.trison"
 delete semi; return new AstReturnStatement(exp->GetLine(),exp->GetScript(),exp);
#line 794 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 19: statement <- RETURN ';':semi    
AstBase* SteelParser::ReductionRuleHandler0019 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 204 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstReturnStatement(line,script);
			
#line 811 "SteelParser.cpp"
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

#line 212 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition, 
						new AstExpression (start->GetLine(),start->GetScript()), stmt); 
			
#line 830 "SteelParser.cpp"
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

#line 218 "steel.trison"

				return new AstLoopStatement(start->GetLine(),start->GetScript(),start,condition,iteration,stmt);
			
#line 850 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 22: statement <- BREAK ';':semi    
AstBase* SteelParser::ReductionRuleHandler0022 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 223 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstBreakStatement(line,script); 
			
#line 867 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 23: statement <- CONTINUE ';':semi    
AstBase* SteelParser::ReductionRuleHandler0023 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 231 "steel.trison"

				int line = semi->GetLine();
				std::string script = semi->GetScript();
				delete semi;
				return new AstContinueStatement(line,script); 
			
#line 884 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 24: exp <- call:call    
AstBase* SteelParser::ReductionRuleHandler0024 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstCallExpression* call = static_cast< AstCallExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 242 "steel.trison"
 return call; 
#line 896 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 25: exp <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0025 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 244 "steel.trison"
 return i;
#line 908 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 26: exp <- FLOAT:f    
AstBase* SteelParser::ReductionRuleHandler0026 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* f = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 246 "steel.trison"
 return f; 
#line 920 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 27: exp <- STRING:s    
AstBase* SteelParser::ReductionRuleHandler0027 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* s = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 248 "steel.trison"
 return s; 
#line 932 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 28: exp <- var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0028 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 250 "steel.trison"
 return id; 
#line 944 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 29: exp <- array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0029 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 252 "steel.trison"
 return id; 
#line 956 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 30: exp <- exp:a '+' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0030 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 254 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::ADD,a,b); 
#line 970 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 31: exp <- exp:a '-' exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0031 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 256 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::SUB,a,b); 
#line 984 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 32: exp <- exp:a '*' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0032 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 258 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MULT,a,b); 
#line 998 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 33: exp <- exp:a '/' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0033 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 260 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::DIV,a,b); 
#line 1012 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 34: exp <- exp:a '%' exp:b    %left %prec MULT_DIV_MOD
AstBase* SteelParser::ReductionRuleHandler0034 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 262 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::MOD,a,b); 
#line 1026 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 35: exp <- exp:a D exp:b    %left %prec POW
AstBase* SteelParser::ReductionRuleHandler0035 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 264 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::D,a,b); 
#line 1040 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 36: exp <- exp:lvalue '=' exp:exp    %right %prec ASSIGNMENT
AstBase* SteelParser::ReductionRuleHandler0036 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 266 "steel.trison"
 return new AstVarAssignmentExpression(lvalue->GetLine(),lvalue->GetScript(),lvalue,exp); 
#line 1054 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 37: exp <- exp:a '^' exp:b    %right %prec POW
AstBase* SteelParser::ReductionRuleHandler0037 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 268 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::POW,a,b); 
#line 1068 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 38: exp <- exp:a OR exp:b    %left %prec OR
AstBase* SteelParser::ReductionRuleHandler0038 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 270 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::OR,a,b); 
#line 1082 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 39: exp <- exp:a AND exp:b    %left %prec AND
AstBase* SteelParser::ReductionRuleHandler0039 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 272 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::AND,a,b); 
#line 1096 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 40: exp <- exp:a EQ exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0040 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 274 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::EQ,a,b); 
#line 1110 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 41: exp <- exp:a NE exp:b    %left %prec EQ_NE
AstBase* SteelParser::ReductionRuleHandler0041 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 276 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::NE,a,b); 
#line 1124 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 42: exp <- exp:a LT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0042 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 278 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LT,a,b); 
#line 1138 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 43: exp <- exp:a GT exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0043 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 280 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GT,a,b); 
#line 1152 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 44: exp <- exp:a LTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0044 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 282 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::LTE,a,b); 
#line 1166 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 45: exp <- exp:a GTE exp:b    %left %prec COMPARATOR
AstBase* SteelParser::ReductionRuleHandler0045 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 284 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::GTE,a,b); 
#line 1180 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 46: exp <- exp:a CAT exp:b    %left %prec ADD_SUB
AstBase* SteelParser::ReductionRuleHandler0046 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* a = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* b = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 286 "steel.trison"
 return new AstBinOp(a->GetLine(),a->GetScript(),AstBinOp::CAT,a,b); 
#line 1194 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 47: exp <- '(' exp:exp ')'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0047 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 288 "steel.trison"
 return exp; 
#line 1206 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 48: exp <- '-' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0048 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 290 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::MINUS,exp); 
#line 1218 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 49: exp <- '+' exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0049 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 292 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::PLUS,exp); 
#line 1230 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 50: exp <- NOT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0050 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 294 "steel.trison"
 return new AstUnaryOp(exp->GetLine(), exp->GetScript(), AstUnaryOp::NOT,exp); 
#line 1242 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 51: exp <- CAT exp:exp     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0051 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 296 "steel.trison"
 return new AstUnaryOp(exp->GetLine(),
exp->GetScript(), AstUnaryOp::CAT,exp); 
#line 1255 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 52: exp <- exp:lvalue '[' exp:index ']'     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0052 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* index = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 299 "steel.trison"
 return new AstArrayElement(lvalue->GetLine(),lvalue->GetScript(),lvalue,index); 
#line 1269 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 53: exp <- INCREMENT exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0053 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 301 "steel.trison"
 return new AstIncrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstIncrement::PRE);
#line 1281 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 54: exp <- exp:lvalue INCREMENT     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0054 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 303 "steel.trison"
 return new AstIncrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstIncrement::POST);
#line 1293 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 55: exp <- DECREMENT exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0055 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 305 "steel.trison"
 return new AstDecrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstDecrement::PRE);
#line 1305 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 56: exp <- exp:lvalue DECREMENT     %prec PAREN
AstBase* SteelParser::ReductionRuleHandler0056 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 307 "steel.trison"
 return new AstDecrement(lvalue->GetLine(),lvalue->GetScript(),lvalue, AstDecrement::POST);
#line 1317 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 57: exp <- POP exp:lvalue     %prec UNARY
AstBase* SteelParser::ReductionRuleHandler0057 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstExpression* lvalue = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 309 "steel.trison"
 return new AstPop(lvalue->GetLine(),lvalue->GetScript(),lvalue); 
#line 1329 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 58: exp_statement <- ';':semi    
AstBase* SteelParser::ReductionRuleHandler0058 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 314 "steel.trison"

			int line = semi->GetLine();
			std::string script = semi->GetScript(); 
			delete semi;
			return new AstExpression(line,script); 
		
#line 1346 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 59: exp_statement <- exp:exp ';':semi    
AstBase* SteelParser::ReductionRuleHandler0059 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(1 < m_reduction_rule_token_count);
    AstBase* semi = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 321 "steel.trison"
 delete semi;  return exp; 
#line 1360 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 60: int_literal <- INT:i    
AstBase* SteelParser::ReductionRuleHandler0060 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* i = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 326 "steel.trison"
 return i; 
#line 1372 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 61: var_identifier <- VAR_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0061 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 331 "steel.trison"
 return id; 
#line 1384 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 62: func_identifier <- FUNC_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0062 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 336 "steel.trison"
 return id; 
#line 1396 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 63: array_identifier <- ARRAY_IDENTIFIER:id    
AstBase* SteelParser::ReductionRuleHandler0063 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstBase* id = static_cast< AstBase* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 341 "steel.trison"
 return id; 
#line 1408 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 64: call <- func_identifier:id '(' ')'    
AstBase* SteelParser::ReductionRuleHandler0064 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 347 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id);
#line 1420 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 65: call <- func_identifier:id '(' param_list:params ')'    
AstBase* SteelParser::ReductionRuleHandler0065 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstFuncIdentifier* id = static_cast< AstFuncIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstParamList* params = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 349 "steel.trison"
 return new AstCallExpression(id->GetLine(),id->GetScript(),id,params);
#line 1434 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 66: vardecl <- VAR var_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0066 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 354 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id);
#line 1446 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 67: vardecl <- VAR var_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0067 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstVarIdentifier * id = static_cast< AstVarIdentifier * >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 356 "steel.trison"
 return new AstVarDeclaration(id->GetLine(),id->GetScript(),id,exp); 
#line 1460 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 68: vardecl <- VAR array_identifier:id '[' exp:i ']'    
AstBase* SteelParser::ReductionRuleHandler0068 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* i = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 358 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id,i); 
#line 1474 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 69: vardecl <- VAR array_identifier:id    
AstBase* SteelParser::ReductionRuleHandler0069 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);

#line 360 "steel.trison"
 return new AstArrayDeclaration(id->GetLine(),id->GetScript(),id); 
#line 1486 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 70: vardecl <- VAR array_identifier:id '=' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0070 ()
{
    assert(1 < m_reduction_rule_token_count);
    AstArrayIdentifier* id = static_cast< AstArrayIdentifier* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 1]);
    assert(3 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 3]);

#line 362 "steel.trison"

							AstArrayDeclaration *pDecl =  new AstArrayDeclaration(id->GetLine(),id->GetScript(),id);
							pDecl->assign(exp);
							return pDecl;
						 
#line 1504 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 71: param_list <- exp:exp    
AstBase* SteelParser::ReductionRuleHandler0071 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);

#line 371 "steel.trison"
 AstParamList * pList = new AstParamList ( exp->GetLine(), exp->GetScript() );
		  pList->add(exp);
		  return pList;
		
#line 1519 "SteelParser.cpp"
    return static_cast<AstBase*>(0);
}

// rule 72: param_list <- param_list:list ',' exp:exp    
AstBase* SteelParser::ReductionRuleHandler0072 ()
{
    assert(0 < m_reduction_rule_token_count);
    AstParamList* list = static_cast< AstParamList* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 0]);
    assert(2 < m_reduction_rule_token_count);
    AstExpression* exp = static_cast< AstExpression* >(m_token_stack[m_token_stack.size() - m_reduction_rule_token_count + 2]);

#line 376 "steel.trison"
 list->add(exp); return list;
#line 1533 "SteelParser.cpp"
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
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0046, "rule 46: exp <- exp CAT exp    %left %prec ADD_SUB"},
    {                  Token::exp__,  3, &SteelParser::ReductionRuleHandler0047, "rule 47: exp <- '(' exp ')'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0048, "rule 48: exp <- '-' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0049, "rule 49: exp <- '+' exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0050, "rule 50: exp <- NOT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0051, "rule 51: exp <- CAT exp     %prec UNARY"},
    {                  Token::exp__,  4, &SteelParser::ReductionRuleHandler0052, "rule 52: exp <- exp '[' exp ']'     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0053, "rule 53: exp <- INCREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0054, "rule 54: exp <- exp INCREMENT     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0055, "rule 55: exp <- DECREMENT exp     %prec UNARY"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0056, "rule 56: exp <- exp DECREMENT     %prec PAREN"},
    {                  Token::exp__,  2, &SteelParser::ReductionRuleHandler0057, "rule 57: exp <- POP exp     %prec UNARY"},
    {        Token::exp_statement__,  1, &SteelParser::ReductionRuleHandler0058, "rule 58: exp_statement <- ';'    "},
    {        Token::exp_statement__,  2, &SteelParser::ReductionRuleHandler0059, "rule 59: exp_statement <- exp ';'    "},
    {          Token::int_literal__,  1, &SteelParser::ReductionRuleHandler0060, "rule 60: int_literal <- INT    "},
    {       Token::var_identifier__,  1, &SteelParser::ReductionRuleHandler0061, "rule 61: var_identifier <- VAR_IDENTIFIER    "},
    {      Token::func_identifier__,  1, &SteelParser::ReductionRuleHandler0062, "rule 62: func_identifier <- FUNC_IDENTIFIER    "},
    {     Token::array_identifier__,  1, &SteelParser::ReductionRuleHandler0063, "rule 63: array_identifier <- ARRAY_IDENTIFIER    "},
    {                 Token::call__,  3, &SteelParser::ReductionRuleHandler0064, "rule 64: call <- func_identifier '(' ')'    "},
    {                 Token::call__,  4, &SteelParser::ReductionRuleHandler0065, "rule 65: call <- func_identifier '(' param_list ')'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0066, "rule 66: vardecl <- VAR var_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0067, "rule 67: vardecl <- VAR var_identifier '=' exp    "},
    {              Token::vardecl__,  5, &SteelParser::ReductionRuleHandler0068, "rule 68: vardecl <- VAR array_identifier '[' exp ']'    "},
    {              Token::vardecl__,  2, &SteelParser::ReductionRuleHandler0069, "rule 69: vardecl <- VAR array_identifier    "},
    {              Token::vardecl__,  4, &SteelParser::ReductionRuleHandler0070, "rule 70: vardecl <- VAR array_identifier '=' exp    "},
    {           Token::param_list__,  1, &SteelParser::ReductionRuleHandler0071, "rule 71: param_list <- exp    "},
    {           Token::param_list__,  3, &SteelParser::ReductionRuleHandler0072, "rule 72: param_list <- param_list ',' exp    "},

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
    {   1,   24,    0,   25,   11}, // state    0
    {   0,    0,   36,    0,    0}, // state    1
    {  37,   14,    0,   51,    5}, // state    2
    {  56,   25,    0,   81,   10}, // state    3
    {  91,   14,    0,  105,    5}, // state    4
    { 110,   14,    0,  124,    5}, // state    5
    { 129,   14,    0,  143,    5}, // state    6
    { 148,    1,    0,    0,    0}, // state    7
    { 149,    1,    0,    0,    0}, // state    8
    { 150,    1,    0,    0,    0}, // state    9
    { 151,   15,    0,  166,    5}, // state   10
    { 171,    1,    0,    0,    0}, // state   11
    { 172,    1,    0,    0,    0}, // state   12
    {   0,    0,  173,    0,    0}, // state   13
    {   0,    0,  174,    0,    0}, // state   14
    {   0,    0,  175,    0,    0}, // state   15
    { 176,    1,    0,    0,    0}, // state   16
    { 177,    2,    0,  179,    2}, // state   17
    {   0,    0,  181,    0,    0}, // state   18
    {   0,    0,  182,    0,    0}, // state   19
    {   0,    0,  183,    0,    0}, // state   20
    { 184,   14,    0,  198,    5}, // state   21
    { 203,   14,    0,  217,    5}, // state   22
    { 222,   14,    0,  236,    5}, // state   23
    { 241,   14,    0,  255,    5}, // state   24
    { 260,    1,    0,    0,    0}, // state   25
    {   0,    0,  261,    0,    0}, // state   26
    { 262,   24,  286,  287,    9}, // state   27
    {   0,    0,  296,    0,    0}, // state   28
    { 297,   21,    0,    0,    0}, // state   29
    {   0,    0,  318,    0,    0}, // state   30
    {   0,    0,  319,    0,    0}, // state   31
    { 320,    1,    0,    0,    0}, // state   32
    {   0,    0,  321,    0,    0}, // state   33
    {   0,    0,  322,    0,    0}, // state   34
    { 323,    1,    0,    0,    0}, // state   35
    { 324,   21,    0,    0,    0}, // state   36
    {   0,    0,  345,    0,    0}, // state   37
    { 346,   25,    0,  371,    9}, // state   38
    { 380,    5,  385,    0,    0}, // state   39
    { 386,    5,  391,    0,    0}, // state   40
    { 392,    5,  397,    0,    0}, // state   41
    { 398,   14,    0,  412,    5}, // state   42
    {   0,    0,  417,    0,    0}, // state   43
    {   0,    0,  418,    0,    0}, // state   44
    {   0,    0,  419,    0,    0}, // state   45
    { 420,   21,    0,    0,    0}, // state   46
    { 441,   14,    0,  455,    5}, // state   47
    { 460,    1,    0,    0,    0}, // state   48
    { 461,   15,    0,  476,    6}, // state   49
    { 482,    1,  483,    0,    0}, // state   50
    { 484,    2,  486,    0,    0}, // state   51
    { 487,    5,  492,    0,    0}, // state   52
    { 493,    5,  498,    0,    0}, // state   53
    { 499,    5,  504,    0,    0}, // state   54
    { 505,    5,  510,    0,    0}, // state   55
    {   0,    0,  511,    0,    0}, // state   56
    {   0,    0,  512,    0,    0}, // state   57
    {   0,    0,  513,    0,    0}, // state   58
    { 514,   14,    0,  528,    5}, // state   59
    { 533,   14,    0,  547,    5}, // state   60
    { 552,   14,    0,  566,    5}, // state   61
    { 571,   14,    0,  585,    5}, // state   62
    { 590,   14,    0,  604,    5}, // state   63
    { 609,   14,    0,  623,    5}, // state   64
    { 628,   14,    0,  642,    5}, // state   65
    { 647,   14,    0,  661,    5}, // state   66
    { 666,   14,    0,  680,    5}, // state   67
    { 685,   14,    0,  699,    5}, // state   68
    { 704,   14,    0,  718,    5}, // state   69
    { 723,   14,    0,  737,    5}, // state   70
    { 742,   14,    0,  756,    5}, // state   71
    { 761,   14,    0,  775,    5}, // state   72
    { 780,   14,    0,  794,    5}, // state   73
    { 799,   14,    0,  813,    5}, // state   74
    { 818,   14,    0,  832,    5}, // state   75
    {   0,    0,  837,    0,    0}, // state   76
    {   0,    0,  838,    0,    0}, // state   77
    { 839,   14,    0,  853,    5}, // state   78
    { 858,   15,    0,  873,    6}, // state   79
    {   0,    0,  879,    0,    0}, // state   80
    {   0,    0,  880,    0,    0}, // state   81
    {   0,    0,  881,    0,    0}, // state   82
    { 882,   21,    0,    0,    0}, // state   83
    {   0,    0,  903,    0,    0}, // state   84
    { 904,   21,    0,    0,    0}, // state   85
    { 925,    2,    0,  927,    2}, // state   86
    { 929,   15,    0,  944,    6}, // state   87
    { 950,   14,    0,  964,    5}, // state   88
    { 969,   14,    0,  983,    5}, // state   89
    { 988,   14,    0, 1002,    5}, // state   90
    {1007,   20, 1027,    0,    0}, // state   91
    {1028,   21,    0,    0,    0}, // state   92
    {1049,    8, 1057,    0,    0}, // state   93
    {1058,    8, 1066,    0,    0}, // state   94
    {1067,    5, 1072,    0,    0}, // state   95
    {1073,    5, 1078,    0,    0}, // state   96
    {1079,    4, 1083,    0,    0}, // state   97
    {1084,    5, 1089,    0,    0}, // state   98
    {1090,    4, 1094,    0,    0}, // state   99
    {1095,   11, 1106,    0,    0}, // state  100
    {1107,   11, 1118,    0,    0}, // state  101
    {1119,   15, 1134,    0,    0}, // state  102
    {1135,   15, 1150,    0,    0}, // state  103
    {1151,   11, 1162,    0,    0}, // state  104
    {1163,   11, 1174,    0,    0}, // state  105
    {1175,   17, 1192,    0,    0}, // state  106
    {1193,   18, 1211,    0,    0}, // state  107
    {1212,    8, 1220,    0,    0}, // state  108
    {   0,    0, 1221,    0,    0}, // state  109
    {1222,   20, 1242,    0,    0}, // state  110
    {1243,    2,    0,    0,    0}, // state  111
    {1245,   24,    0, 1269,    9}, // state  112
    {1278,   24,    0, 1302,    9}, // state  113
    {1311,    1,    0,    0,    0}, // state  114
    {1312,    2,    0,    0,    0}, // state  115
    {   0,    0, 1314,    0,    0}, // state  116
    {1315,   15,    0, 1330,    5}, // state  117
    {1335,   20, 1355,    0,    0}, // state  118
    {1356,   20, 1376,    0,    0}, // state  119
    {1377,   21,    0,    0,    0}, // state  120
    {   0,    0, 1398,    0,    0}, // state  121
    {   0,    0, 1399,    0,    0}, // state  122
    {1400,   14,    0, 1414,    5}, // state  123
    {   0,    0, 1419,    0,    0}, // state  124
    {1420,    1, 1421,    0,    0}, // state  125
    {1422,   24,    0, 1446,   10}, // state  126
    {1456,    1,    0,    0,    0}, // state  127
    {1457,    1,    0, 1458,    1}, // state  128
    {1459,   24,    0, 1483,    9}, // state  129
    {1492,   21,    0,    0,    0}, // state  130
    {   0,    0, 1513,    0,    0}, // state  131
    {1514,   20, 1534,    0,    0}, // state  132
    {1535,   24,    0, 1559,    9}, // state  133
    {1568,   25,    0, 1593,    9}, // state  134
    {1602,   24,    0, 1626,   10}, // state  135
    {   0,    0, 1636,    0,    0}, // state  136
    {   0,    0, 1637,    0,    0}, // state  137
    {1638,   24,    0, 1662,    9}, // state  138
    {   0,    0, 1671,    0,    0}, // state  139
    {   0,    0, 1672,    0,    0}, // state  140
    {1673,   25,    0, 1698,    9}, // state  141
    {   0,    0, 1707,    0,    0}, // state  142
    {   0,    0, 1708,    0,    0}  // state  143

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
    // nonterminal transitions
    {                 Token::root__, {                  TA_PUSH_STATE,   25}},
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   27}},
    {            Token::statement__, {                  TA_PUSH_STATE,   28}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state    1
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   58}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   36}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state    3
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   37}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,   38}},
    {            Token::statement__, {                  TA_PUSH_STATE,   28}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   39}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   40}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   41}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state    7
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state    8
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state    9
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state   10
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   45}},
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   46}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   11
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   12
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {        Token::FUNC_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state   13
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   62}},

// ///////////////////////////////////////////////////////////////////////////
// state   14
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   61}},

// ///////////////////////////////////////////////////////////////////////////
// state   15
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   63}},

// ///////////////////////////////////////////////////////////////////////////
// state   16
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   17
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {         Token::VAR_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   14}},
    {       Token::ARRAY_IDENTIFIER, {        TA_SHIFT_AND_PUSH_STATE,   15}},
    // nonterminal transitions
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   50}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   51}},

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
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   52}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   53}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   54}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   55}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   25
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::END_, {        TA_SHIFT_AND_PUSH_STATE,   56}},

// ///////////////////////////////////////////////////////////////////////////
// state   26
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   11}},

// ///////////////////////////////////////////////////////////////////////////
// state   27
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
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    1}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,   57}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   28
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    8}},

// ///////////////////////////////////////////////////////////////////////////
// state   29
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   58}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   30
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   10}},

// ///////////////////////////////////////////////////////////////////////////
// state   31
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   28}},

// ///////////////////////////////////////////////////////////////////////////
// state   32
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   79}},

// ///////////////////////////////////////////////////////////////////////////
// state   33
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   29}},

// ///////////////////////////////////////////////////////////////////////////
// state   34
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   24}},

// ///////////////////////////////////////////////////////////////////////////
// state   35
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   80}},

// ///////////////////////////////////////////////////////////////////////////
// state   36
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,   81}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   37
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   13}},

// ///////////////////////////////////////////////////////////////////////////
// state   38
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,   82}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,   57}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state   39
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   48}},

// ///////////////////////////////////////////////////////////////////////////
// state   40
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   49}},

// ///////////////////////////////////////////////////////////////////////////
// state   41
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   50}},

// ///////////////////////////////////////////////////////////////////////////
// state   42
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   83}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   43
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   22}},

// ///////////////////////////////////////////////////////////////////////////
// state   44
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   23}},

// ///////////////////////////////////////////////////////////////////////////
// state   45
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   19}},

// ///////////////////////////////////////////////////////////////////////////
// state   46
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,   84}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   47
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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   48
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,   86}},

// ///////////////////////////////////////////////////////////////////////////
// state   49
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   87}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   50
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   88}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   66}},

// ///////////////////////////////////////////////////////////////////////////
// state   51
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   89}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   90}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   69}},

// ///////////////////////////////////////////////////////////////////////////
// state   52
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   53}},

// ///////////////////////////////////////////////////////////////////////////
// state   53
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   55}},

// ///////////////////////////////////////////////////////////////////////////
// state   54
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   51}},

// ///////////////////////////////////////////////////////////////////////////
// state   55
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   57}},

// ///////////////////////////////////////////////////////////////////////////
// state   56
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {TA_REDUCE_AND_ACCEPT_USING_RULE,    0}},

// ///////////////////////////////////////////////////////////////////////////
// state   57
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    9}},

// ///////////////////////////////////////////////////////////////////////////
// state   58
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   59}},

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
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   91}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {                  Token::exp__, {                  TA_PUSH_STATE,   92}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   93}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   94}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   95}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   96}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   97}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   98}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,   99}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  100}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  101}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  102}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  103}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  104}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  105}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  106}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  107}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   76
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   54}},

// ///////////////////////////////////////////////////////////////////////////
// state   77
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   56}},

// ///////////////////////////////////////////////////////////////////////////
// state   78
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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   79
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  109}},
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
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {           Token::param_list__, {                  TA_PUSH_STATE,  111}},

// ///////////////////////////////////////////////////////////////////////////
// state   80
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   14}},

// ///////////////////////////////////////////////////////////////////////////
// state   81
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   47}},

// ///////////////////////////////////////////////////////////////////////////
// state   82
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   12}},

// ///////////////////////////////////////////////////////////////////////////
// state   83
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  112}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   84
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   18}},

// ///////////////////////////////////////////////////////////////////////////
// state   85
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  113}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   86
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  114}},
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {     Token::param_definition__, {                  TA_PUSH_STATE,  115}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,  116}},

// ///////////////////////////////////////////////////////////////////////////
// state   87
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
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,  117}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   88
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  118}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   89
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  119}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

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
    {                  Token::exp__, {                  TA_PUSH_STATE,  120}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   91
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   36}},

// ///////////////////////////////////////////////////////////////////////////
// state   92
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  121}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state   93
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   31}},

// ///////////////////////////////////////////////////////////////////////////
// state   94
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   30}},

// ///////////////////////////////////////////////////////////////////////////
// state   95
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   32}},

// ///////////////////////////////////////////////////////////////////////////
// state   96
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   33}},

// ///////////////////////////////////////////////////////////////////////////
// state   97
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   37}},

// ///////////////////////////////////////////////////////////////////////////
// state   98
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state   99
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  100
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   43}},

// ///////////////////////////////////////////////////////////////////////////
// state  101
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   42}},

// ///////////////////////////////////////////////////////////////////////////
// state  102
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   40}},

// ///////////////////////////////////////////////////////////////////////////
// state  103
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   41}},

// ///////////////////////////////////////////////////////////////////////////
// state  104
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   45}},

// ///////////////////////////////////////////////////////////////////////////
// state  105
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   44}},

// ///////////////////////////////////////////////////////////////////////////
// state  106
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   39}},

// ///////////////////////////////////////////////////////////////////////////
// state  107
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   38}},

// ///////////////////////////////////////////////////////////////////////////
// state  108
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   46}},

// ///////////////////////////////////////////////////////////////////////////
// state  109
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   64}},

// ///////////////////////////////////////////////////////////////////////////
// state  110
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   71}},

// ///////////////////////////////////////////////////////////////////////////
// state  111
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  122}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  123}},

// ///////////////////////////////////////////////////////////////////////////
// state  112
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,  124}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  113
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,  125}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  114
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  126}},

// ///////////////////////////////////////////////////////////////////////////
// state  115
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  127}},
    {              Token::Type(','), {        TA_SHIFT_AND_PUSH_STATE,  128}},

// ///////////////////////////////////////////////////////////////////////////
// state  116
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    6}},

// ///////////////////////////////////////////////////////////////////////////
// state  117
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  129}},
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  130}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state  118
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   67}},

// ///////////////////////////////////////////////////////////////////////////
// state  119
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   70}},

// ///////////////////////////////////////////////////////////////////////////
// state  120
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type(']'), {        TA_SHIFT_AND_PUSH_STATE,  131}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state  121
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   52}},

// ///////////////////////////////////////////////////////////////////////////
// state  122
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   65}},

// ///////////////////////////////////////////////////////////////////////////
// state  123
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
    {                  Token::exp__, {                  TA_PUSH_STATE,  132}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},

// ///////////////////////////////////////////////////////////////////////////
// state  124
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   15}},

// ///////////////////////////////////////////////////////////////////////////
// state  125
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                   Token::ELSE, {        TA_SHIFT_AND_PUSH_STATE,  133}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   17}},

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
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   21}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   22}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  134}},
    {            Token::statement__, {                  TA_PUSH_STATE,   28}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  127
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,  135}},

// ///////////////////////////////////////////////////////////////////////////
// state  128
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {                    Token::VAR, {        TA_SHIFT_AND_PUSH_STATE,   17}},
    // nonterminal transitions
    {              Token::vardecl__, {                  TA_PUSH_STATE,  136}},

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
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   23}},
    {                    Token::POP, {        TA_SHIFT_AND_PUSH_STATE,   24}},
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,  137}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  130
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(')'), {        TA_SHIFT_AND_PUSH_STATE,  138}},
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},

// ///////////////////////////////////////////////////////////////////////////
// state  131
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   68}},

// ///////////////////////////////////////////////////////////////////////////
// state  132
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type('='), {        TA_SHIFT_AND_PUSH_STATE,   59}},
    {              Token::Type('['), {        TA_SHIFT_AND_PUSH_STATE,   60}},
    {              Token::Type('-'), {        TA_SHIFT_AND_PUSH_STATE,   61}},
    {              Token::Type('+'), {        TA_SHIFT_AND_PUSH_STATE,   62}},
    {              Token::Type('*'), {        TA_SHIFT_AND_PUSH_STATE,   63}},
    {              Token::Type('/'), {        TA_SHIFT_AND_PUSH_STATE,   64}},
    {              Token::Type('^'), {        TA_SHIFT_AND_PUSH_STATE,   65}},
    {              Token::Type('%'), {        TA_SHIFT_AND_PUSH_STATE,   66}},
    {                      Token::D, {        TA_SHIFT_AND_PUSH_STATE,   67}},
    {                     Token::GT, {        TA_SHIFT_AND_PUSH_STATE,   68}},
    {                     Token::LT, {        TA_SHIFT_AND_PUSH_STATE,   69}},
    {                     Token::EQ, {        TA_SHIFT_AND_PUSH_STATE,   70}},
    {                     Token::NE, {        TA_SHIFT_AND_PUSH_STATE,   71}},
    {                    Token::GTE, {        TA_SHIFT_AND_PUSH_STATE,   72}},
    {                    Token::LTE, {        TA_SHIFT_AND_PUSH_STATE,   73}},
    {                    Token::AND, {        TA_SHIFT_AND_PUSH_STATE,   74}},
    {                     Token::OR, {        TA_SHIFT_AND_PUSH_STATE,   75}},
    {              Token::INCREMENT, {        TA_SHIFT_AND_PUSH_STATE,   76}},
    {              Token::DECREMENT, {        TA_SHIFT_AND_PUSH_STATE,   77}},
    {                    Token::CAT, {        TA_SHIFT_AND_PUSH_STATE,   78}},
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   72}},

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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,  139}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  134
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  140}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,   57}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  135
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {       Token::statement_list__, {                  TA_PUSH_STATE,  141}},
    {            Token::statement__, {                  TA_PUSH_STATE,   28}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  136
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    7}},

// ///////////////////////////////////////////////////////////////////////////
// state  137
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   20}},

// ///////////////////////////////////////////////////////////////////////////
// state  138
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,  142}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  139
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   16}},

// ///////////////////////////////////////////////////////////////////////////
// state  140
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,    3}},

// ///////////////////////////////////////////////////////////////////////////
// state  141
// ///////////////////////////////////////////////////////////////////////////
    // terminal transitions
    {              Token::Type(';'), {        TA_SHIFT_AND_PUSH_STATE,    1}},
    {              Token::Type('('), {        TA_SHIFT_AND_PUSH_STATE,    2}},
    {              Token::Type('{'), {        TA_SHIFT_AND_PUSH_STATE,    3}},
    {              Token::Type('}'), {        TA_SHIFT_AND_PUSH_STATE,  143}},
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
    // nonterminal transitions
    {      Token::func_definition__, {                  TA_PUSH_STATE,   26}},
    {            Token::statement__, {                  TA_PUSH_STATE,   57}},
    {                  Token::exp__, {                  TA_PUSH_STATE,   29}},
    {        Token::exp_statement__, {                  TA_PUSH_STATE,   30}},
    {       Token::var_identifier__, {                  TA_PUSH_STATE,   31}},
    {      Token::func_identifier__, {                  TA_PUSH_STATE,   32}},
    {     Token::array_identifier__, {                  TA_PUSH_STATE,   33}},
    {                 Token::call__, {                  TA_PUSH_STATE,   34}},
    {              Token::vardecl__, {                  TA_PUSH_STATE,   35}},

// ///////////////////////////////////////////////////////////////////////////
// state  142
// ///////////////////////////////////////////////////////////////////////////
    // default transition
    {               Token::DEFAULT_, {           TA_REDUCE_USING_RULE,   21}},

// ///////////////////////////////////////////////////////////////////////////
// state  143
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

#line 4329 "SteelParser.cpp"

