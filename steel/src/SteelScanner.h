// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY
// SteelScanner.h generated by reflex at Sun Jan  5 13:20:11 2014
// from steel.reflex using reflex.cpp.targetspec and reflex.cpp.header.codespec
// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY

#include <cassert>
#include <deque>
#include <string>

#if !defined(BarfCpp_namespace_)
#define BarfCpp_namespace_
namespace BarfCpp_ {

// /////////////////////////////////////////////////////////////////////////////
// a bunch of template metaprogramming to intelligently determine what type to
// use for an integer of the given bit width or value range.
// /////////////////////////////////////////////////////////////////////////////

template <bool condition, typename Then, typename Else> struct If;
template <typename Then, typename Else> struct If<true,Then,Else> { typedef Then T; };
template <typename Then, typename Else> struct If<false,Then,Else> { typedef Else T; };

template <bool condition> struct Assert;
template <> struct Assert<true> { static bool const v = true; operator bool () { return v; } };

template <typename Sint, typename Uint> struct IntPair { typedef Sint S; typedef Uint U; };

template <int bits> struct Integer
{
private:

    typedef
        typename If<bits == 8*sizeof(char),      IntPair<char,unsigned char>,
        typename If<bits == 8*sizeof(short),     IntPair<short,unsigned short>,
        typename If<bits == 8*sizeof(int),       IntPair<int,unsigned int>,
        typename If<bits == 8*sizeof(long),      IntPair<long,unsigned long>,
        typename If<bits == 8*sizeof(long long), IntPair<long long,unsigned long long>,
        Integer<0> // if no match, cause a compile error
        >::T >::T >::T >::T >::T PrivateIntPair;
    static bool const assert_size =
        Assert<bits == 8*sizeof(typename PrivateIntPair::S) &&
               bits == 8*sizeof(typename PrivateIntPair::U)>::v;

public:

    typedef typename PrivateIntPair::S Signed;
    typedef typename PrivateIntPair::U Unsigned;
};
template <> struct Integer<0> { }; // empty for intentional compile errors

// /////////////////////////////////////////////////////////////////////////////
// use the above to define specific integer types.  you could trivially add
// 64-bit integers here, but for the purposes of this file, there is no reason
// to use them (though on a 64-bit machine, Diff and Size WILL be 64-bit).
// /////////////////////////////////////////////////////////////////////////////

typedef Integer<8> ::Signed                Sint8;
typedef Integer<8> ::Unsigned              Uint8;
typedef Integer<16>::Signed                Sint16;
typedef Integer<16>::Unsigned              Uint16;
typedef Integer<32>::Signed                Sint32;
typedef Integer<32>::Unsigned              Uint32;
typedef Integer<8*sizeof(void*)>::Signed   Diff; // difference between pointers
typedef Integer<8*sizeof(void*)>::Unsigned Size; // size of blocks of memory

// /////////////////////////////////////////////////////////////////////////////
// here are a few compile-time assertions to check that the integers actually
// turned out to be the right sizes.
// /////////////////////////////////////////////////////////////////////////////

enum
{
    TYPE_SIZE_ASSERTIONS =
        Assert<sizeof(Sint8)  == 1>::v &&
        Assert<sizeof(Uint8)  == 1>::v &&
        Assert<sizeof(Sint16) == 2>::v &&
        Assert<sizeof(Uint16) == 2>::v &&
        Assert<sizeof(Sint32) == 4>::v &&
        Assert<sizeof(Uint32) == 4>::v &&
        Assert<sizeof(Diff)   == sizeof(void*)>::v &&
        Assert<sizeof(Size)   == sizeof(void*)>::v
};

} // end of namespace BarfCpp_
#endif // !defined(BarfCpp_namespace_)

#if !defined(ReflexCpp_namespace_)
#define ReflexCpp_namespace_
namespace ReflexCpp_ {

// /////////////////////////////////////////////////////////////////////////////
// implements the InputApparatus interface as described in the documentation
// /////////////////////////////////////////////////////////////////////////////

class InputApparatus_
{
protected:

    typedef bool (InputApparatus_::*IsInputAtEndMethod_)();
    typedef BarfCpp_::Uint8 (InputApparatus_::*ReadNextAtomMethod_)();

    InputApparatus_ (IsInputAtEndMethod_ IsInputAtEnd, ReadNextAtomMethod_ ReadNextAtom)
        :
        m_IsInputAtEnd(IsInputAtEnd),
        m_ReadNextAtom(ReadNextAtom)
    {
        // subclasses must call InputApparatus_::ResetForNewInput_ in their constructors.
    }

    bool IsAtEndOfInput () { return IsConditionalMet(CF_END_OF_INPUT, CF_END_OF_INPUT); }

    void PrepareToScan_ ()
    {
        assert(m_start_cursor < m_read_cursor);
        // update the read and keep_string cursors.  if KeepString was called,
        // this should do nothing, since in that case, m_start_cursor will be
        // zero;  if Unaccept was called, it should have postcondition
        // m_read_cursor == m_start_cursor + 1
        m_read_cursor -= m_start_cursor;
        m_kept_string_cursor -= m_start_cursor;
        // dump the first m_start_cursor chars from the buffer
        while (m_start_cursor > 0)
        {
            --m_start_cursor;
            m_buffer.pop_front();
        }
        // reset the accept cursor
        m_accept_cursor = m_start_cursor;
        m_keep_string_has_been_called = false;
        assert(m_kept_string_cursor > m_start_cursor);
        assert(m_accept_cursor == 0);
    }
    void ResetForNewInput_ ()
    {
        m_current_conditional_flags = 0;
        m_buffer.clear();
        m_buffer.push_front('\0'); // special "previous" atom for beginning of input
        assert(m_buffer.size() == 1);
        m_start_cursor = 0;
        m_read_cursor = 1;
        m_kept_string_cursor = 1;
        m_accept_cursor = 0;
        m_keep_string_has_been_called = false;
    }

    // for use in AutomatonApparatus_ only
    BarfCpp_::Uint8 CurrentConditionalFlags_ ()
    {
        UpdateConditionalFlags();
        return m_current_conditional_flags;
    }
    BarfCpp_::Uint8 InputAtom_ ()
    {
        FillBuffer();
        assert(m_read_cursor > 0);
        assert(m_read_cursor < m_buffer.size());
        return m_buffer[m_read_cursor];
    }
    void AdvanceReadCursor_ ()
    {
        assert(m_start_cursor == 0);
        assert(m_read_cursor > 0);
        assert(m_read_cursor < m_buffer.size());
        if (m_buffer[m_read_cursor] != '\0')
            ++m_read_cursor;
    }
    void SetAcceptCursor_ ()
    {
        assert(m_start_cursor == 0);
        assert(m_read_cursor > 0);
        assert(m_read_cursor <= m_buffer.size());
        m_accept_cursor = m_read_cursor;
    }
    void Accept_ (std::string &s)
    {
        assert(m_accept_cursor > 0 && "can't Accept_ if the accept cursor is not set");
        assert(m_accept_cursor <= m_read_cursor);
        AcceptRejectCommon(s);
    }
    void Reject_ (std::string &s)
    {
        assert(m_accept_cursor == 0 && "can't Reject_ if the accept cursor was set");
        // must set the accept cursor to indicate the rejected string
        // (which is the kept string plus the rejected atom)
        m_accept_cursor = m_kept_string_cursor;
        assert(m_accept_cursor < m_buffer.size());
        if (m_buffer[m_accept_cursor] != '\0')
            ++m_accept_cursor;
        AcceptRejectCommon(s);
    }

private:

    void AcceptRejectCommon (std::string &s)
    {
        assert(s.empty());
        assert(m_buffer.size() >= 2);
        assert(m_start_cursor == 0);
        // the accept cursor indicates the end of the string to accept/reject
        assert(m_accept_cursor > 0 && m_accept_cursor <= m_buffer.size());
        // there should not be an EOF-indicating '\0' at the end of the string
        assert(m_accept_cursor == 1 || m_buffer[m_accept_cursor-1] != '\0');
        // extract the accepted/rejected string: range [1,m_accept_cursor).
        s.insert(s.begin(), m_buffer.begin()+1, m_buffer.begin()+m_accept_cursor);
        assert(s.size() == m_accept_cursor-1);
        // set the start cursor to one before the end of the string
        // (the last char in the string becomes the previous atom)
        m_start_cursor = m_accept_cursor - 1;
        // reset the other cursors
        m_read_cursor = m_accept_cursor;
        m_kept_string_cursor = m_accept_cursor;
    }

    enum ConditionalFlag
    {
        CF_BEGINNING_OF_INPUT = (1 << 0),
        CF_END_OF_INPUT       = (1 << 1),
        CF_BEGINNING_OF_LINE  = (1 << 2),
        CF_END_OF_LINE        = (1 << 3),
        CF_WORD_BOUNDARY      = (1 << 4)
    }; // end of enum ReflexCpp_::InputApparatus_::ConditionalFlag

    bool IsConditionalMet (BarfCpp_::Uint8 conditional_mask, BarfCpp_::Uint8 conditional_flags)
    {
        UpdateConditionalFlags();
        // return true iff no bits indicated by conditional_mask differ between
        // conditional_flags and m_current_conditional_flags.
        return ((conditional_flags ^ m_current_conditional_flags) & conditional_mask) == 0;
    }
    void FillBuffer ()
    {
        assert(m_read_cursor > 0);
        assert(m_read_cursor <= m_buffer.size());
        // if we already have at least one atom ahead of the read cursor in
        // the input buffer, there is no need to suck another one out.
        if (m_read_cursor < m_buffer.size())
            return;
        // if the last atom (in front of the read cursor) in the input buffer
        // is '\0' then we have reached EOF, so there is no need to suck
        // another atom out.
        if (m_buffer.size() >= 2 && *m_buffer.rbegin() == '\0')
        {
            assert(m_read_cursor < m_buffer.size());
            return;
        }

        // if we're at the end of input, push a null char
        if ((this->*m_IsInputAtEnd)())
            m_buffer.push_back('\0');
        // otherwise retrieve and push the next input atom
        else
        {
            BarfCpp_::Uint8 atom = (this->*m_ReadNextAtom)();
            assert(atom != '\0' && "may not return '\\0' from return_next_input_char");
            m_buffer.push_back(atom);
        }

        // ensure there is at least one atom on each side of the read cursor.
        assert(m_buffer.size() >= 2);
        assert(m_read_cursor < m_buffer.size());
    }
    void UpdateConditionalFlags ()
    {
        FillBuffer();
        assert(m_read_cursor > 0);
        assert(m_read_cursor < m_buffer.size());
        // given the atoms surrounding the read cursor, calculate the
        // current conditional flags.
        m_current_conditional_flags = 0;
        if (m_buffer[m_read_cursor-1] == '\0')                                            m_current_conditional_flags |= CF_BEGINNING_OF_INPUT;
        if (m_buffer[m_read_cursor] == '\0')                                              m_current_conditional_flags |= CF_END_OF_INPUT;
        if (m_buffer[m_read_cursor-1] == '\0' || m_buffer[m_read_cursor-1] == '\n')       m_current_conditional_flags |= CF_BEGINNING_OF_LINE;
        if (m_buffer[m_read_cursor] == '\0' || m_buffer[m_read_cursor] == '\n')           m_current_conditional_flags |= CF_END_OF_LINE;
        if (IsWordChar(m_buffer[m_read_cursor-1]) != IsWordChar(m_buffer[m_read_cursor])) m_current_conditional_flags |= CF_WORD_BOUNDARY;
    }
    static bool IsWordChar (BarfCpp_::Uint8 c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_'; }

    typedef std::deque<BarfCpp_::Uint8> Buffer;

    BarfCpp_::Uint8 m_current_conditional_flags;
    Buffer m_buffer;
    // indicates the "previous" atom (of the kept string)
    Buffer::size_type m_start_cursor;
    // indicates how far the scanner has read
    Buffer::size_type m_read_cursor;
    // indicates the end of the kept string
    Buffer::size_type m_kept_string_cursor;
    // if != m_start_cursor, indicates the most recent, longest accepted string
    Buffer::size_type m_accept_cursor;
    // indicates if KeepString has been called during the accept/reject handler
    bool m_keep_string_has_been_called;
    // corresponds to the directive return_true_iff_input_is_at_end
    IsInputAtEndMethod_ m_IsInputAtEnd;
    // corresponds to the directive return_next_input_char
    ReadNextAtomMethod_ m_ReadNextAtom;
}; // end of class ReflexCpp_::InputApparatus_

// /////////////////////////////////////////////////////////////////////////////
// implements the AutomatonApparatus interface as described in the documentation
// -- it contains all the generalized state machinery for running a reflex DFA.
// /////////////////////////////////////////////////////////////////////////////

class AutomatonApparatus_ : protected InputApparatus_
{
protected:

    // state machine mode flags
    enum
    {
        MF_CASE_INSENSITIVE_ = (1 << 0),
        MF_UNGREEDY_         = (1 << 1)
    };

    struct DfaTransition_;
    struct DfaState_
    {
        BarfCpp_::Uint32 m_accept_handler_index;
        BarfCpp_::Size m_transition_count;
        DfaTransition_ const *m_transition;
    }; // end of struct ReflexCpp_::AutomatonApparatus_::DfaState_
    struct DfaTransition_
    {
        enum Type
        {
            INPUT_ATOM = 0, INPUT_ATOM_RANGE, CONDITIONAL
        }; // end of enum ReflexCpp_::AutomatonApparatus_::DfaTransition_::Type

        BarfCpp_::Uint8 m_transition_type;
        BarfCpp_::Uint8 m_data_0;
        BarfCpp_::Uint8 m_data_1;
        DfaState_ const *m_target_dfa_state;

        bool AcceptsInputAtom (BarfCpp_::Uint8 input_atom, bool is_case_insensitive) const
        {
            assert(m_transition_type == INPUT_ATOM || m_transition_type == INPUT_ATOM_RANGE);
            // returns true iff this transition is INPUT_ATOM and input_atom
            // matches m_data_0, or this transition is INPUT_ATOM_RANGE and
            // input_atom is within the range [m_data_0, m_data_1] inclusive.
            if (is_case_insensitive)
            {
                BarfCpp_::Uint8 switched_case_input_atom = SwitchCase(input_atom);
                return (m_transition_type == INPUT_ATOM
                        &&
                        (m_data_0 == input_atom || m_data_0 == switched_case_input_atom))
                       ||
                       (m_transition_type == INPUT_ATOM_RANGE
                        &&
                        ((m_data_0 <= input_atom && input_atom <= m_data_1)
                         ||
                         (m_data_0 <= switched_case_input_atom && switched_case_input_atom <= m_data_1)));
            }
            else // case sensitive
                return (m_transition_type == INPUT_ATOM &&
                        m_data_0 == input_atom)
                       ||
                       (m_transition_type == INPUT_ATOM_RANGE &&
                        m_data_0 <= input_atom && input_atom <= m_data_1);
        }
        bool AcceptsConditionalFlags (BarfCpp_::Uint8 conditional_flags) const
        {
            assert(m_transition_type == CONDITIONAL);
            // returns true iff this transition is CONDITIONAL and no relevant bits
            // in conditional_flags conflict with this transition's conditional mask
            // (m_data_0) and flags (m_data_1).
            return ((conditional_flags ^ m_data_1) & m_data_0) == 0;
        }
        static BarfCpp_::Uint8 SwitchCase (BarfCpp_::Uint8 c)
        {
            if (c >= 'a' && c <= 'z')
                return c - 'a' + 'A';
            if (c >= 'A' && c <= 'Z')
                return c - 'A' + 'a';
            return c;
        }
    }; // end of struct ReflexCpp_::AutomatonApparatus_::DfaTransition_

    AutomatonApparatus_ (
        DfaState_ const *state_table,
        BarfCpp_::Size state_count,
        DfaTransition_ const *transition_table,
        BarfCpp_::Size transition_count,
        BarfCpp_::Uint32 accept_handler_count,
        IsInputAtEndMethod_ IsInputAtEnd,
        ReadNextAtomMethod_ ReadNextAtom)
        :
        InputApparatus_(IsInputAtEnd, ReadNextAtom),
        m_accept_handler_count(accept_handler_count)
    {
        CheckDfa(state_table, state_count, transition_table, transition_count);
        // subclasses must call ReflexCpp_::InputApparatus_::ResetForNewInput_ in their constructors.
    }

    DfaState_ const *InitialState_ () const
    {
        return m_initial_state;
    }
    void InitialState_ (DfaState_ const *initial_state)
    {
        assert(initial_state != NULL);
        m_initial_state = initial_state;
    }
    void ModeFlags_ (BarfCpp_::Uint8 mode_flags)
    {
        m_mode_flags = mode_flags;
    }
    void ResetForNewInput_ (DfaState_ const *initial_state, BarfCpp_::Uint8 mode_flags)
    {
        InputApparatus_::ResetForNewInput_();
        if (initial_state != NULL)
            InitialState_(initial_state);
        m_current_state = NULL;
        m_accept_state = NULL;
        m_mode_flags = mode_flags;
    }
    BarfCpp_::Uint32 RunDfa_ (std::string &s)
    {
        assert(s.empty());
        // reset the current state to the initial state.
        assert(m_initial_state != NULL);
        m_current_state = m_initial_state;
        assert(m_accept_state == NULL);
        // loop until there are no valid transitions from the current state.
        while (m_current_state != NULL)
        {
            // if the current state is an accept state, save it
            if (IsAcceptState(m_current_state))
            {
                m_accept_state = m_current_state;
                SetAcceptCursor_();
                // if we're in ungreedy mode, accept the shortest string
                // possible; don't process any more input.
                if ((m_mode_flags & MF_UNGREEDY_) != 0)
                {
                    assert(m_accept_state != NULL);
                    m_current_state = NULL;
                    break;
                }
            }
            // turn the crank on the state machine, exercising the appropriate
            // conditional (using m_current_conditional_flags) or atomic
            // transition (using the atom at the read cursor in the buffer).
            // m_current_state will be set to the transitioned-to state, or
            // NULL if no transition was possible.
            m_current_state = ProcessInputAtom();
        }
        // if we have a most recent accept state, accept the accumulated input
        // using the accept handler indicated by the most recent accept state.
        if (m_accept_state != NULL)
        {
            // extract the accepted string from the buffer
            Accept_(s);
            // save off the accept handler index
            BarfCpp_::Uint32 accept_handler_index = m_accept_state->m_accept_handler_index;
            // clear the accept state for next time
            m_accept_state = NULL;
            // return accept_handler_index to indicate which handler to call
            return accept_handler_index;
        }
        // otherwise the input atom went unhandled; extract the rejected string.
        else
        {
            // put the rejected string in the return string and indicate
            // that no accept handler should be called.
            Reject_(s);
            return m_accept_handler_count;
        }
    }

private:

    // these InputApparatus_ methods should not be accessable to SteelScanner
    using InputApparatus_::CurrentConditionalFlags_;
    using InputApparatus_::InputAtom_;
    using InputApparatus_::AdvanceReadCursor_;
    using InputApparatus_::SetAcceptCursor_;
    using InputApparatus_::Accept_;
    using InputApparatus_::Reject_;

    DfaState_ const *ProcessInputAtom ()
    {
        assert(m_current_state != NULL);
        // get the current conditional flags and input atom once before looping
        BarfCpp_::Uint8 current_conditional_flags = CurrentConditionalFlags_();
        BarfCpp_::Uint8 input_atom = InputAtom_();
        // calculate the case sensitivity
        bool is_case_insensitive = (m_mode_flags & MF_CASE_INSENSITIVE_) != 0;
        // iterate through the current state's transitions, exercising the first
        // acceptable one and returning the target state
        for (DfaTransition_ const *transition = m_current_state->m_transition,
                                  *transition_end = transition + m_current_state->m_transition_count;
             transition != transition_end;
             ++transition)
        {
            assert(transition->m_transition_type == DfaTransition_::INPUT_ATOM ||
                   transition->m_transition_type == DfaTransition_::INPUT_ATOM_RANGE ||
                   transition->m_transition_type == DfaTransition_::CONDITIONAL);
            // if it's an atomic transition, check if it accepts input_atom.
            if (transition->m_transition_type == DfaTransition_::INPUT_ATOM ||
                transition->m_transition_type == DfaTransition_::INPUT_ATOM_RANGE)
            {
                if (transition->AcceptsInputAtom(input_atom, is_case_insensitive))
                {
                    AdvanceReadCursor_();
                    return transition->m_target_dfa_state;
                }
            }
            // otherwise it must be a conditional transition, so check the flags.
            else if (transition->AcceptsConditionalFlags(current_conditional_flags))
                return transition->m_target_dfa_state;
        }
        // if we reached here, no transition was possible, so return NULL.
        return NULL;
    }
    bool IsAcceptState (DfaState_ const *state) const
    {
        assert(state != NULL);
        return state->m_accept_handler_index < m_accept_handler_count;
    }
    static void CheckDfa (
        DfaState_ const *state_table,
        BarfCpp_::Size state_count,
        DfaTransition_ const *transition_table,
        BarfCpp_::Size transition_count)
    {
        // if any assertions in this method fail, the state and/or
        // transition tables were created incorrectly.
        assert(state_table != NULL && "must have a state table");
        assert(state_count > 0 && "must have at least one state");
        assert(transition_table != NULL && "must have a transition table");
        {
            DfaTransition_ const *t = transition_table;
            for (DfaState_ const *s = state_table, *s_end = state_table + state_count;
                 s != s_end;
                 ++s)
            {
                assert(s->m_transition == t &&
                       "states' transitions must be contiguous and in ascending order");
                t += s->m_transition_count;
            }
            assert(t == transition_table + transition_count &&
                   "there are too many or too few referenced "
                   "transitions in the state table");
        }
        for (DfaTransition_ const *t = transition_table,
                                  *t_end = transition_table + transition_count;
             t != t_end;
             ++t)
        {
            assert((t->m_transition_type == DfaTransition_::INPUT_ATOM ||
                    t->m_transition_type == DfaTransition_::INPUT_ATOM_RANGE ||
                    t->m_transition_type == DfaTransition_::CONDITIONAL)
                   &&
                   "invalid DfaTransition_::Type");
            assert(t->m_target_dfa_state >= state_table &&
                   t->m_target_dfa_state < state_table + state_count &&
                   "transition target state out of range "
                   "(does not point to a valid state)");
            if (t->m_transition_type == DfaTransition_::INPUT_ATOM_RANGE)
            {
                assert(t->m_data_0 < t->m_data_1 &&
                       "can't specify a single-element range of atoms");
            }
            else if (t->m_transition_type == DfaTransition_::CONDITIONAL)
            {
                assert(t->m_data_0 != 0 &&
                       "can't have a conditional with a mask of zero");
                assert((t->m_data_1 & ~t->m_data_0) == 0 &&
                       "there are bits set in the conditional flags "
                       "which are outside of the conditional mask");
            }
        }
    }

    BarfCpp_::Uint32 const m_accept_handler_count;
    DfaState_ const *m_initial_state;
    DfaState_ const *m_current_state;
    DfaState_ const *m_accept_state;
    BarfCpp_::Uint8 m_mode_flags;
}; // end of class ReflexCpp_::AutomatonApparatus_

} // end of namespace ReflexCpp_
#endif // !defined(ReflexCpp_namespace_)


#line 6 "steel.reflex"


#include <cassert>
#include <sstream>
#ifndef _STEEL_SCANNER_H
#define _STEEL_SCANNER_H

#include "SteelParser.h"
namespace Steel { 
class AstBase;

#line 598 "SteelScanner.h"

class SteelScanner : private ReflexCpp_::AutomatonApparatus_
{
public:

    struct StateMachine
    {
        enum Name
        {
            COMMENT_GUTS,
            MAIN,
            STRING_LITERAL_GUTS,
            // default starting state machine
            START_ = MAIN
        }; // end of enum SteelScanner::StateMachine::Name
    }; // end of struct SteelScanner::StateMachine


#line 33 "steel.reflex"

	void setBuffer(const char * pBuffer, const std::string &name);
	void append(const std::string& str);
	int getCurrentLine() const { return m_line; }
	std::string getScriptName() const { return m_script_name; }

#line 624 "SteelScanner.h"

public:

    SteelScanner ();
    ~SteelScanner ();

    bool DebugSpew () const { return m_debug_spew_; }
    void DebugSpew (bool debug_spew) { m_debug_spew_ = debug_spew; }

    StateMachine::Name CurrentStateMachine () const;
    void SwitchToStateMachine (StateMachine::Name state_machine);

    using AutomatonApparatus_::IsAtEndOfInput;
    void ResetForNewInput ();

    SteelParser::Token Scan () throw();

public:


#line 40 "steel.reflex"


	// Converters
	int ToInt(const std::string &text);
	double ToFloat(const std::string &text);
	int ToIntFromHex(const std::string &text);
        int NewlineCount(const std::string &text);
	std::stringstream m_stream;
	unsigned int m_line;
	AstBase * m_token;
	std::string m_script_name;

#line 658 "SteelScanner.h"


private:

    // ///////////////////////////////////////////////////////////////////////
    // begin internal reflex-generated parser guts -- don't use
    // ///////////////////////////////////////////////////////////////////////

    using InputApparatus_::PrepareToScan_;
    using InputApparatus_::ResetForNewInput_;

    using AutomatonApparatus_::InitialState_;
    using AutomatonApparatus_::ResetForNewInput_;
    using AutomatonApparatus_::RunDfa_;

    bool IsInputAtEnd_ () throw();
    BarfCpp_::Uint8 ReadNextAtom_ () throw();

    // debug spew methods
    static void PrintAtom_ (BarfCpp_::Uint8 atom);
    static void PrintString_ (std::string const &s);

    bool m_debug_spew_;

    // state machine and automaton data
    static BarfCpp_::Uint32 const ms_state_machine_start_state_index_[];
    static BarfCpp_::Uint8 const ms_state_machine_mode_flags_[];
    static char const *const ms_state_machine_name_[];
    static BarfCpp_::Uint32 const ms_state_machine_count_;
    static AutomatonApparatus_::DfaState_ const ms_state_table_[];
    static BarfCpp_::Size const ms_state_count_;
    static AutomatonApparatus_::DfaTransition_ const ms_transition_table_[];
    static BarfCpp_::Size const ms_transition_count_;
    static char const *const ms_accept_handler_regex_[];
    static BarfCpp_::Uint32 const ms_accept_handler_count_;

    // ///////////////////////////////////////////////////////////////////////
    // end of internal reflex-generated parser guts
    // ///////////////////////////////////////////////////////////////////////
}; // end of class SteelScanner


#line 18 "steel.reflex"

} // namespace
#endif // _STEEL_SCANNER_H

#line 706 "SteelScanner.h"
