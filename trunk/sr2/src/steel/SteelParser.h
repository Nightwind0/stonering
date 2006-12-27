#include <ostream>
#include <string>
#include <vector>


#line 6 "steel.trison"
 
	#ifndef STEEL_PARSER_H
	#define STEEL_PARSER_H

	#include <cassert> 
	class SteelScanner;

#line 15 "SteelParser.h"

class SteelParser

{
public:

    struct Token
    {
        enum Type
        {
            // user-defined terminal, non-single-character tokens
            AND = 0x100,
            ARRAY_IDENTIFIER,
            BREAK,
            CONTINUE,
            D,
            DECREMENT,
            ELSE,
            EQ,
            FLOAT,
            FUNCTION,
            FUNC_IDENTIFIER,
            GT,
            GTE,
            IF,
            INCREMENT,
            INT,
            LOOP,
            LT,
            LTE,
            NE,
            NOT,
            OR,
            RETURN,
            STRING,
            TIMES,
            USING,
            VAR,
            VAR_IDENTIFIER,
            WHILE,

            // special end-of-input terminal
            END_,

            // user-defined nonterminal tokens
            call__,
            exp__,
            func_definition__,
            func_definition_list__,
            param_definition__,
            param_list__,
            root__,
            statement__,
            statement_list__,
            vardecl__,

            // special start nonterminal
            START_,

            // parser's sentinel and special tokens
            ERROR_,
            DEFAULT_,
            INVALID_
        }; // end of enum SteelParser::Token::Type
    }; // end of struct SteelParser::Token

    SteelParser ();
    ~SteelParser ();

    inline int const &GetAcceptedToken () const { return m_reduction_token; }
    inline void ClearAcceptedToken () { m_reduction_token = static_cast<int>(0); }

    inline unsigned int GetDebugSpewLevel () const { return m_debug_spew_level; }
    inline void SetDebugSpewLevel (unsigned int debug_spew_level) { m_debug_spew_level = debug_spew_level; }

    static void CheckStateConsistency ();

    enum ParserReturnCode
    {
        PRC_SUCCESS = 0,
        PRC_UNHANDLED_PARSE_ERROR = 1
    }; // end of enum ParserReturnCode

public:

    ParserReturnCode Parse ();

public:


#line 2 "steel.trison"

    Token::Type Scan ();
    SteelScanner *m_scanner;

#line 111 "SteelParser.h"

private:

    typedef unsigned int StateNumber;

    enum TransitionAction
    {
        TA_SHIFT_AND_PUSH_STATE = 0,
        TA_PUSH_STATE,
        TA_REDUCE_USING_RULE,
        TA_REDUCE_AND_ACCEPT_USING_RULE,
        TA_THROW_AWAY_LOOKAHEAD_TOKEN,

        TA_COUNT
    };

    enum ActionReturnCode
    {
        ARC_CONTINUE_PARSING = 0,
        ARC_ACCEPT_AND_RETURN
    };

    struct ReductionRule
    {
        typedef int (SteelParser::*ReductionRuleHandler)();

        Token::Type m_non_terminal_to_reduce_to;
        unsigned int m_number_of_tokens_to_reduce_by;
        ReductionRuleHandler m_handler;
        std::string m_description;
    };

    struct Action
    {
        TransitionAction m_transition_action;
        unsigned int m_data;
    };

    struct StateTransition
    {
        Token::Type m_token_type;
        Action m_action;
    };

    struct State
    {
        unsigned int m_lookahead_transition_offset;
        unsigned int m_lookahead_transition_count;
        unsigned int m_default_action_offset;
        unsigned int m_non_terminal_transition_offset;
        unsigned int m_non_terminal_transition_count;
    };

    inline void GetNewLookaheadToken ()
    {
        if (m_is_new_lookahead_token_required)
        {
            m_is_new_lookahead_token_required = false;
            if (m_get_new_lookahead_token_type_from_saved)
            {
                m_get_new_lookahead_token_type_from_saved = false;
                m_lookahead_token_type = m_saved_lookahead_token_type;
            }
            else
                ScanANewLookaheadToken();
        }
    }
    inline Token::Type GetLookaheadTokenType ()
    {
        GetNewLookaheadToken();
        return m_lookahead_token_type;
    }
    inline int const &GetLookaheadToken ()
    {
        GetNewLookaheadToken();
        return m_lookahead_token;
    }
    bool GetDoesStateAcceptErrorToken (StateNumber state_number) const;

    ParserReturnCode PrivateParse ();

    ActionReturnCode ProcessAction (Action const &action);
    void ShiftLookaheadToken ();
    void PushState (StateNumber state_number);
    void ReduceUsingRule (ReductionRule const &reduction_rule, bool and_accept);
    void PopStates (unsigned int number_of_states_to_pop, bool print_state_stack = true);
    void PrintStateStack () const;
    void PrintTokenStack () const;
    void PrintStateTransition (unsigned int state_transition_number) const;
    void ScanANewLookaheadToken ();
    void ThrowAwayToken (int token);
    void ThrowAwayTokenStack ();

    typedef std::vector<StateNumber> StateStack;
    typedef std::vector< int > TokenStack;

    unsigned int m_debug_spew_level;

    StateStack m_state_stack;
    TokenStack m_token_stack;

    Token::Type m_lookahead_token_type;
    int m_lookahead_token;
    bool m_is_new_lookahead_token_required;

    Token::Type m_saved_lookahead_token_type;
    bool m_get_new_lookahead_token_type_from_saved;
    bool m_previous_transition_accepted_error_token;

    bool m_is_returning_with_non_terminal;
    Token::Type m_returning_with_this_non_terminal;

    int m_reduction_token;
    unsigned int m_reduction_rule_token_count;

    static State const ms_state[];
    static unsigned int const ms_state_count;
    static ReductionRule const ms_reduction_rule[];
    static unsigned int const ms_reduction_rule_count;
    static StateTransition const ms_state_transition[];
    static unsigned int const ms_state_transition_count;

    int ReductionRuleHandler0000 ();
    int ReductionRuleHandler0001 ();
    int ReductionRuleHandler0002 ();
    int ReductionRuleHandler0003 ();
    int ReductionRuleHandler0004 ();
    int ReductionRuleHandler0005 ();
    int ReductionRuleHandler0006 ();
    int ReductionRuleHandler0007 ();
    int ReductionRuleHandler0008 ();
    int ReductionRuleHandler0009 ();
    int ReductionRuleHandler0010 ();
    int ReductionRuleHandler0011 ();
    int ReductionRuleHandler0012 ();
    int ReductionRuleHandler0013 ();
    int ReductionRuleHandler0014 ();
    int ReductionRuleHandler0015 ();
    int ReductionRuleHandler0016 ();
    int ReductionRuleHandler0017 ();
    int ReductionRuleHandler0018 ();
    int ReductionRuleHandler0019 ();
    int ReductionRuleHandler0020 ();
    int ReductionRuleHandler0021 ();
    int ReductionRuleHandler0022 ();
    int ReductionRuleHandler0023 ();
    int ReductionRuleHandler0024 ();
    int ReductionRuleHandler0025 ();
    int ReductionRuleHandler0026 ();
    int ReductionRuleHandler0027 ();
    int ReductionRuleHandler0028 ();
    int ReductionRuleHandler0029 ();
    int ReductionRuleHandler0030 ();
    int ReductionRuleHandler0031 ();
    int ReductionRuleHandler0032 ();
    int ReductionRuleHandler0033 ();
    int ReductionRuleHandler0034 ();
    int ReductionRuleHandler0035 ();
    int ReductionRuleHandler0036 ();
    int ReductionRuleHandler0037 ();
    int ReductionRuleHandler0038 ();
    int ReductionRuleHandler0039 ();
    int ReductionRuleHandler0040 ();
    int ReductionRuleHandler0041 ();
    int ReductionRuleHandler0042 ();
    int ReductionRuleHandler0043 ();
    int ReductionRuleHandler0044 ();
    int ReductionRuleHandler0045 ();
    int ReductionRuleHandler0046 ();
    int ReductionRuleHandler0047 ();
    int ReductionRuleHandler0048 ();
    int ReductionRuleHandler0049 ();
    int ReductionRuleHandler0050 ();
    int ReductionRuleHandler0051 ();
    int ReductionRuleHandler0052 ();
    int ReductionRuleHandler0053 ();
    int ReductionRuleHandler0054 ();
    int ReductionRuleHandler0055 ();
    int ReductionRuleHandler0056 ();
    int ReductionRuleHandler0057 ();
    int ReductionRuleHandler0058 ();
    int ReductionRuleHandler0059 ();

}; // end of class SteelParser

std::ostream &operator << (std::ostream &stream, SteelParser::Token::Type token_type);


#line 13 "steel.trison"

	#endif // STEEL_PARSER_H

#line 304 "SteelParser.h"
