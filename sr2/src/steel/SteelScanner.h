// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY
// SteelScanner.h generated by reflex at Fri Jan 19 22:09:54 2007
// from steel.reflex using reflex.cpp.langspec and reflex.cpp.header.codespec
// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY

#include <set>
#include <string>


#line 3 "steel.reflex"


#ifndef _STEEL_SCANNER_H
#define _STEEL_SCANNER_H
#include "SteelParser.h"
class AstBase;

#line 19 "SteelScanner.h"

class SteelScanner
{
public:

#line 26 "steel.reflex"

	void setBuffer(const char * pBuffer, const std::string &name);

#line 29 "SteelScanner.h"

public:

    // this must be unsigned for certain range checks
    typedef unsigned char InputAtom;
    typedef std::string InputContainer;

    enum
    {
        DEBUG_NONE                         = 0x00,

        DEBUG_CURRENT_STATE_SET            = 0x01,
        DEBUG_MOST_RECENT_ACCEPT_STATE_SET = 0x02,
        DEBUG_ACCEPT_STATE                 = 0x04,
        DEBUG_CURRENT_ACCUMULATED_INPUT    = 0x08,
        DEBUG_INPUT_BUFFER                 = 0x10,
        DEBUG_TRANSITION_TO_STATE          = 0x20,

        DEBUG_ALL                          = 0x3F
    };

    SteelScanner ();
    ~SteelScanner ();

    inline unsigned int GetDebugSpewFlags () const { return m_debug_spew_flags; }
    inline void SetDebugSpewFlags (unsigned int debug_spew_flags) { m_debug_spew_flags = debug_spew_flags; }

    static void PerformStateMachineConsistencyCheck ();


    SteelParser::Token::Type Scan (AstBase **token);

private:

    typedef SteelParser::Token::Type (SteelScanner::*AcceptHandler)(InputContainer const &);

    struct Transition;

    struct State
    {
        static unsigned int const ms_accept_state_count = 40;

        enum Name
        {
            MAIN = 40,
            STRING_LITERAL_GUTS = 110,
        }; // end of enum SteelScanner::State::Name

        unsigned int m_transition_count;
        Transition const *m_transition;

        inline unsigned int GetAcceptStateIndex () const
        {
            assert(this >= SteelScanner::ms_state);
            assert(this < SteelScanner::ms_state + ms_accept_state_count);
            return this - SteelScanner::ms_state;
        }
        inline bool IsAcceptState () const
        {
            assert(this >= SteelScanner::ms_state);
            assert(this < SteelScanner::ms_state + SteelScanner::ms_state_count);
            return (unsigned int)(this - SteelScanner::ms_state) < ms_accept_state_count;
        }
    }; // end of struct SteelScanner::State

    struct Transition
    {
        enum Type
        {
            TT_INPUT_ATOM = 0,
            TT_INPUT_ATOM_RANGE,
            TT_EPSILON,
            // custom transition types go here
            TT_BEGINNING_OF_INPUT,
            TT_NOT_BEGINNING_OF_INPUT,
            TT_END_OF_INPUT,
            TT_NOT_END_OF_INPUT,
            TT_BEGINNING_OF_LINE,
            TT_NOT_BEGINNING_OF_LINE,
            TT_END_OF_LINE,
            TT_NOT_END_OF_LINE,
            TT_WORD_BOUNDARY,
            TT_NOT_WORD_BOUNDARY,
            // end custom transition types
            TT_COUNT
        }; // end of enum SteelScanner::Transition::Type

        Type m_type;
        // value for TT_INPUT_ATOM and bottom of range for TT_INPUT_ATOM_RANGE
        InputAtom m_input_atom_range_lower;
        // top of range for TT_INPUT_ATOM_RANGE
        InputAtom m_input_atom_range_upper;
        State const *m_target_state;

        inline bool IsConditional () const { return m_type > TT_EPSILON; }
        bool operator == (InputAtom input_atom) const;
    }; // end of struct SteelScanner::Transition

    typedef std::set<State const *> StateSet;

    bool IsConditionMet (Transition::Type transition_condition);
    bool IsInputAtEnd ();
    InputAtom ReadFromInput ();
    void RetrieveNextInputAtom ();
    void HandleUnmatchedCharacter (InputAtom unmatched_character);
    void TransitionToState (State::Name scanner_state_initial_index);
    void ResetForNewInput ();
    void PerformClosureOfCurrentStateSet ();
    void PerformClosureOfState (State const &state, StateSet *new_state_set);
    void ProcessInput ();
    bool AnyStatesInTheCurrentStateSetAreAcceptStates () const;
    void UpdateMostRecentAcceptStateSet ();

    static void PrintStateSet (std::ostream &stream, std::string const &message, StateSet const &state_set);

    // input/output members
    unsigned int m_debug_spew_flags;
    // nfa-related members
    bool m_has_past_beginning_of_input;
    InputContainer m_input_buffer;
    InputContainer m_accumulated_input;
    InputAtom m_previous_input_atom;
    InputAtom m_current_input_atom;
    unsigned int m_initial_state_index;
    StateSet m_current_state_set;
    StateSet m_most_recent_accept_state_set;
    unsigned int m_most_recent_accept_input_length;
    // input condition members
    bool m_is_beginning_of_input;
    bool m_is_end_of_input;
    bool m_is_beginning_of_line;
    bool m_is_end_of_line;
    bool m_is_word_boundary;

    static State const ms_state[];
    static unsigned int const ms_state_count;

    static Transition const ms_transition[];
    static unsigned int const ms_transition_count;

public:

#line 30 "steel.reflex"


	// Converters
	int ToInt(const std::string &text);
	double ToFloat(const std::string &text);
	int ToIntFromHex(const std::string &text);

	unsigned int m_char_cursor;
	const char * m_pBuffer;
	unsigned int m_line;
	std::string m_script_name;

#line 185 "SteelScanner.h"

}; // end of class SteelScanner


#line 11 "steel.reflex"

#endif // _STEEL_SCANNER_H

#line 194 "SteelScanner.h"
