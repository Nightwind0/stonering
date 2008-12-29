#include <ostream>
#include <string>
#include <vector>


#line 15 "steel.trison"
 
	#ifndef STEEL_PARSER_H
	#define STEEL_PARSER_H

	#include <cassert> 
	#include <string>
	#include "Ast.h"
	class SteelScanner;

#line 17 "SteelParser.h"

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
            BAREWORD,
            BOOLEAN,
            BREAK,
            CAT,
            CONSTANT,
            CONTINUE,
            D,
            DECREMENT,
            DO,
            ELSE,
            EQ,
            FINAL,
            FLOAT,
            FOR,
            FUNCTION,
            GT,
            GTE,
            IF,
            IMPORT,
            INCREMENT,
            INT,
            LT,
            LTE,
            NE,
            NOT,
            OR,
            POP,
            RETURN,
            SCOPE,
            STRING,
            VAR,
            VAR_IDENTIFIER,
            WHILE,

            // special end-of-input terminal
            END_,

            // user-defined nonterminal tokens
            array_identifier__,
            call__,
            exp__,
            exp_statement__,
            func_definition__,
            func_identifier__,
            int_literal__,
            param_definition__,
            param_id__,
            param_list__,
            root__,
            statement__,
            statement_list__,
            var_identifier__,
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

    inline AstBase* const &GetAcceptedToken () const { return m_reduction_token; }
    inline void ClearAcceptedToken () { m_reduction_token = static_cast<AstBase*>(0); }

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


#line 3 "steel.trison"

    void setBuffer(const char *pBuffer, const std::string &script_name);
	bool hadError() const { return mbErrorEncountered; }
	std::string getErrors() const { return mErrors; }
    Token::Type Scan ();
private:
	void addError(unsigned int line, const std::string &error);
    SteelScanner *m_scanner;
    const char *m_pBuffer;
	bool mbErrorEncountered;
	std::string mErrors;	 

#line 132 "SteelParser.h"

private:

    typedef unsigned int StateNumber;

    enum TransitionAction
    {
        TA_SHIFT_AND_PUSH_STATE = 0,
        TA_PUSH_STATE,
        TA_REDUCE_USING_RULE,
        TA_REDUCE_AND_ACCEPT_USING_RULE,

        TA_COUNT
    };

    enum ActionReturnCode
    {
        ARC_CONTINUE_PARSING = 0,
        ARC_ACCEPT_AND_RETURN
    };

    struct ReductionRule
    {
        typedef AstBase* (SteelParser::*ReductionRuleHandler)();

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
            ScanANewLookaheadToken();
        }
    }
    inline Token::Type GetLookaheadTokenType ()
    {
        GetNewLookaheadToken();
        return m_lookahead_token_type;
    }
    inline AstBase* const &GetLookaheadToken ()
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
    void ThrowAwayToken (AstBase* token);
    void ThrowAwayTokenStack ();

    typedef std::vector<StateNumber> StateStack;
    typedef std::vector< AstBase* > TokenStack;

    unsigned int m_debug_spew_level;

    StateStack m_state_stack;
    TokenStack m_token_stack;

    Token::Type m_lookahead_token_type;
    AstBase* m_lookahead_token;
    bool m_is_new_lookahead_token_required;
    bool m_in_error_handling_mode;

    bool m_is_returning_with_non_terminal;
    Token::Type m_returning_with_this_non_terminal;

    AstBase* m_reduction_token;
    unsigned int m_reduction_rule_token_count;

    static State const ms_state[];
    static unsigned int const ms_state_count;
    static ReductionRule const ms_reduction_rule[];
    static unsigned int const ms_reduction_rule_count;
    static StateTransition const ms_state_transition[];
    static unsigned int const ms_state_transition_count;

    AstBase* ReductionRuleHandler0000 ();
    AstBase* ReductionRuleHandler0001 ();
    AstBase* ReductionRuleHandler0002 ();
    AstBase* ReductionRuleHandler0003 ();
    AstBase* ReductionRuleHandler0004 ();
    AstBase* ReductionRuleHandler0005 ();
    AstBase* ReductionRuleHandler0006 ();
    AstBase* ReductionRuleHandler0007 ();
    AstBase* ReductionRuleHandler0008 ();
    AstBase* ReductionRuleHandler0009 ();
    AstBase* ReductionRuleHandler0010 ();
    AstBase* ReductionRuleHandler0011 ();
    AstBase* ReductionRuleHandler0012 ();
    AstBase* ReductionRuleHandler0013 ();
    AstBase* ReductionRuleHandler0014 ();
    AstBase* ReductionRuleHandler0015 ();
    AstBase* ReductionRuleHandler0016 ();
    AstBase* ReductionRuleHandler0017 ();
    AstBase* ReductionRuleHandler0018 ();
    AstBase* ReductionRuleHandler0019 ();
    AstBase* ReductionRuleHandler0020 ();
    AstBase* ReductionRuleHandler0021 ();
    AstBase* ReductionRuleHandler0022 ();
    AstBase* ReductionRuleHandler0023 ();
    AstBase* ReductionRuleHandler0024 ();
    AstBase* ReductionRuleHandler0025 ();
    AstBase* ReductionRuleHandler0026 ();
    AstBase* ReductionRuleHandler0027 ();
    AstBase* ReductionRuleHandler0028 ();
    AstBase* ReductionRuleHandler0029 ();
    AstBase* ReductionRuleHandler0030 ();
    AstBase* ReductionRuleHandler0031 ();
    AstBase* ReductionRuleHandler0032 ();
    AstBase* ReductionRuleHandler0033 ();
    AstBase* ReductionRuleHandler0034 ();
    AstBase* ReductionRuleHandler0035 ();
    AstBase* ReductionRuleHandler0036 ();
    AstBase* ReductionRuleHandler0037 ();
    AstBase* ReductionRuleHandler0038 ();
    AstBase* ReductionRuleHandler0039 ();
    AstBase* ReductionRuleHandler0040 ();
    AstBase* ReductionRuleHandler0041 ();
    AstBase* ReductionRuleHandler0042 ();
    AstBase* ReductionRuleHandler0043 ();
    AstBase* ReductionRuleHandler0044 ();
    AstBase* ReductionRuleHandler0045 ();
    AstBase* ReductionRuleHandler0046 ();
    AstBase* ReductionRuleHandler0047 ();
    AstBase* ReductionRuleHandler0048 ();
    AstBase* ReductionRuleHandler0049 ();
    AstBase* ReductionRuleHandler0050 ();
    AstBase* ReductionRuleHandler0051 ();
    AstBase* ReductionRuleHandler0052 ();
    AstBase* ReductionRuleHandler0053 ();
    AstBase* ReductionRuleHandler0054 ();
    AstBase* ReductionRuleHandler0055 ();
    AstBase* ReductionRuleHandler0056 ();
    AstBase* ReductionRuleHandler0057 ();
    AstBase* ReductionRuleHandler0058 ();
    AstBase* ReductionRuleHandler0059 ();
    AstBase* ReductionRuleHandler0060 ();
    AstBase* ReductionRuleHandler0061 ();
    AstBase* ReductionRuleHandler0062 ();
    AstBase* ReductionRuleHandler0063 ();
    AstBase* ReductionRuleHandler0064 ();
    AstBase* ReductionRuleHandler0065 ();
    AstBase* ReductionRuleHandler0066 ();
    AstBase* ReductionRuleHandler0067 ();
    AstBase* ReductionRuleHandler0068 ();
    AstBase* ReductionRuleHandler0069 ();
    AstBase* ReductionRuleHandler0070 ();
    AstBase* ReductionRuleHandler0071 ();
    AstBase* ReductionRuleHandler0072 ();
    AstBase* ReductionRuleHandler0073 ();
    AstBase* ReductionRuleHandler0074 ();
    AstBase* ReductionRuleHandler0075 ();
    AstBase* ReductionRuleHandler0076 ();
    AstBase* ReductionRuleHandler0077 ();
    AstBase* ReductionRuleHandler0078 ();
    AstBase* ReductionRuleHandler0079 ();
    AstBase* ReductionRuleHandler0080 ();
    AstBase* ReductionRuleHandler0081 ();
    AstBase* ReductionRuleHandler0082 ();
    AstBase* ReductionRuleHandler0083 ();
    AstBase* ReductionRuleHandler0084 ();
    AstBase* ReductionRuleHandler0085 ();
    AstBase* ReductionRuleHandler0086 ();
    AstBase* ReductionRuleHandler0087 ();
    AstBase* ReductionRuleHandler0088 ();
    AstBase* ReductionRuleHandler0089 ();
    AstBase* ReductionRuleHandler0090 ();
    AstBase* ReductionRuleHandler0091 ();
    AstBase* ReductionRuleHandler0092 ();
    AstBase* ReductionRuleHandler0093 ();
    AstBase* ReductionRuleHandler0094 ();
    AstBase* ReductionRuleHandler0095 ();
    AstBase* ReductionRuleHandler0096 ();
    AstBase* ReductionRuleHandler0097 ();
    AstBase* ReductionRuleHandler0098 ();
    AstBase* ReductionRuleHandler0099 ();
    AstBase* ReductionRuleHandler0100 ();
    AstBase* ReductionRuleHandler0101 ();
    AstBase* ReductionRuleHandler0102 ();
    AstBase* ReductionRuleHandler0103 ();
    AstBase* ReductionRuleHandler0104 ();
    AstBase* ReductionRuleHandler0105 ();
    AstBase* ReductionRuleHandler0106 ();
    AstBase* ReductionRuleHandler0107 ();
    AstBase* ReductionRuleHandler0108 ();
    AstBase* ReductionRuleHandler0109 ();
    AstBase* ReductionRuleHandler0110 ();
    AstBase* ReductionRuleHandler0111 ();
    AstBase* ReductionRuleHandler0112 ();
    AstBase* ReductionRuleHandler0113 ();
    AstBase* ReductionRuleHandler0114 ();
    AstBase* ReductionRuleHandler0115 ();
    AstBase* ReductionRuleHandler0116 ();
    AstBase* ReductionRuleHandler0117 ();
    AstBase* ReductionRuleHandler0118 ();
    AstBase* ReductionRuleHandler0119 ();
    AstBase* ReductionRuleHandler0120 ();
    AstBase* ReductionRuleHandler0121 ();
    AstBase* ReductionRuleHandler0122 ();
    AstBase* ReductionRuleHandler0123 ();

}; // end of class SteelParser

std::ostream &operator << (std::ostream &stream, SteelParser::Token::Type token_type);


#line 24 "steel.trison"

	#endif // STEEL_PARSER_H

#line 379 "SteelParser.h"
