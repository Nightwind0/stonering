// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY
// SteelScanner.cpp generated by reflex at Thu Dec 28 00:37:11 2006
// from steel.reflex using reflex.cpp.langspec and reflex.cpp.implementation.codespec
// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY

#include "SteelScanner.h"

#include <cassert>
#include <cctype>
#include <iostream>

#define DEBUG_SPEW(debug_spew_flag, spew_code) if (m_debug_spew_flags&debug_spew_flag) { spew_code; }


#line 15 "steel.reflex"

#include <iostream>
#include <sstream>
#include "SteelScanner.h"
#include "Ast.h"

#line 23 "SteelScanner.cpp"

bool SteelScanner::Transition::operator == (InputAtom input_atom) const
{
    // implementation-specific code
    return m_type == TT_INPUT_ATOM && m_input_atom_range_lower == input_atom
           ||
           m_type == TT_INPUT_ATOM_RANGE && m_input_atom_range_lower <= input_atom && input_atom <= m_input_atom_range_upper;
}

SteelScanner::SteelScanner ()
{
    // one-time initializations
    m_debug_spew_flags = 0;
    std::cin.unsetf(std::ios_base::skipws);
    // per-input initializations
    ResetForNewInput();


#line 80 "steel.reflex"

	m_char_cursor = 0;
	m_line = 1;
	m_script_name = "UNKNOWN SCRIPT";	
	m_pBuffer = "";

#line 49 "SteelScanner.cpp"
}

SteelScanner::~SteelScanner ()
{
}

SteelParser::Token::Type SteelScanner::Scan (AstBase **token)
{
    bool just_accepted;
    do
    {
        // every-time initializations, including setting the initial state.
        m_accumulated_input.clear();
        m_current_state_set.clear();
        m_most_recent_accept_state_set.clear();
        m_most_recent_accept_input_length = 0;
        m_current_state_set.insert(ms_state+m_initial_state_index);
        just_accepted = false;

        while (!m_current_state_set.empty())
        {
            // retrieve the next piece of input.
            RetrieveNextInputAtom();
            // this call performs epsilon-closure on the current state as well
            // as closure for conditionals (e.g. word-boundaries in regexes).
            PerformClosureOfCurrentStateSet();
            // we want to save off any accept states from the current state set,
            // so that if the next input fails on all transitions, we can accept
            // using the most-recently known accept state(s).
            if (AnyStatesInTheCurrentStateSetAreAcceptStates())
                UpdateMostRecentAcceptStateSet();

            DEBUG_SPEW(DEBUG_CURRENT_STATE_SET, PrintStateSet(std::cerr, "current state set", m_current_state_set))

            // turn the crank on the state machine, advancing states which accept
            // the next input atom, and removing states which do not.
            ProcessInput();
            // if any states accepted the input in the above call to ProcessInput(),
            // then add it to the accumulated input container, removing it from the
            // input buffer.
            if (!m_current_state_set.empty())
            {
                assert(!m_input_buffer.empty());
                m_accumulated_input.push_back(m_input_buffer[0]);
                m_input_buffer.erase(0, 1);
            }

            DEBUG_SPEW(DEBUG_CURRENT_ACCUMULATED_INPUT, std::cerr << "current accumulated input = \"" << m_accumulated_input << "\"" << std::endl);
        }

        // if we have at least one accept state, accept the accumulated
        // input using the highest priority accept state.
        if (!m_most_recent_accept_state_set.empty())
        {
            // put all the input atoms which are not within the first
            // m_most_recent_accept_input_length atoms of m_accumulated_input
            // back into m_input_buffer
            assert(m_most_recent_accept_input_length <= m_accumulated_input.length());
            m_input_buffer.insert(0, m_accumulated_input.c_str()+m_most_recent_accept_input_length);
            m_accumulated_input.resize(m_most_recent_accept_input_length);

            // the first state in the most recent accept state set has the
            // highest priority, because the set is ordered from highest
            // priority to lowest priority (it works out that the pointer
            // values are in exactly the same relative order as the
            // priorities).
            State const *accept_state = *m_most_recent_accept_state_set.begin();
            assert(accept_state != NULL);
            assert(accept_state->IsAcceptState());
            assert(accept_state >= ms_state && accept_state < ms_state + State::ms_accept_state_count);

            // execute the appropriate accept handler
            std::string const &accepted_text = m_accumulated_input;
            DEBUG_SPEW(DEBUG_ACCEPT_STATE, std::cerr << "accepting using state " << accept_state - ms_state << ", text = \"" << accepted_text << "\"" << std::endl)
            switch (accept_state->GetAcceptStateIndex())
            {
                case 0:
                {

#line 124 "steel.reflex"

		return SteelParser::Token::END_;
	
#line 133 "SteelScanner.cpp"

                }
                break;

                case 1:
                {

#line 129 "steel.reflex"

		m_line++;
	
#line 145 "SteelScanner.cpp"

                }
                break;

                case 2:
                {

#line 134 "steel.reflex"

		// Eat it
	 
#line 157 "SteelScanner.cpp"

                }
                break;

                case 3:
                {

#line 139 "steel.reflex"

		*token = new AstString(m_line,m_script_name);
		
		TransitionToState(State::STRING_LITERAL_GUTS);
	
#line 171 "SteelScanner.cpp"

                }
                break;

                case 4:
                {

#line 146 "steel.reflex"

		*token = new AstInteger(m_line,m_script_name,ToInt(accepted_text));
			
		return SteelParser::Token::INT;
	
#line 185 "SteelScanner.cpp"

                }
                break;

                case 5:
                {

#line 153 "steel.reflex"

		*token = new AstInteger(m_line,m_script_name,ToIntFromHex(accepted_text));
		
		return SteelParser::Token::INT;
	
#line 199 "SteelScanner.cpp"

                }
                break;

                case 6:
                {

#line 160 "steel.reflex"

		*token = new AstFloat(m_line,m_script_name,ToFloat(accepted_text));
		return SteelParser::Token::FLOAT;
	
#line 212 "SteelScanner.cpp"

                }
                break;

                case 7:
                {

#line 166 "steel.reflex"

		*token = new AstVarIdentifier(m_line,m_script_name,accepted_text);
		return SteelParser::Token::VAR_IDENTIFIER;		
	
#line 225 "SteelScanner.cpp"

                }
                break;

                case 8:
                {

#line 172 "steel.reflex"

		*token = new AstArrayIdentifier(m_line,m_script_name,accepted_text);
		return SteelParser::Token::ARRAY_IDENTIFIER;
	
#line 238 "SteelScanner.cpp"

                }
                break;

                case 9:
                {

#line 178 "steel.reflex"

		return (SteelParser::Token::Type)accepted_text[0];
	
#line 250 "SteelScanner.cpp"

                }
                break;

                case 10:
                {

#line 183 "steel.reflex"

		 *token = new AstKeyword(m_line,m_script_name);
		 return SteelParser::Token::LTE; 
	
#line 263 "SteelScanner.cpp"

                }
                break;

                case 11:
                {

#line 189 "steel.reflex"

		 *token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::GTE; 
	
#line 275 "SteelScanner.cpp"

                }
                break;

                case 12:
                {

#line 194 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name);
		return SteelParser::Token::NE;
	
#line 288 "SteelScanner.cpp"

                }
                break;

                case 13:
                {

#line 200 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::EQ;
	 
#line 300 "SteelScanner.cpp"

                }
                break;

                case 14:
                {

#line 205 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::LT; 
	
#line 312 "SteelScanner.cpp"

                }
                break;

                case 15:
                {

#line 210 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::GT; 
	
#line 324 "SteelScanner.cpp"

                }
                break;

                case 16:
                {

#line 215 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::D;
	 
#line 336 "SteelScanner.cpp"

                }
                break;

                case 17:
                {

#line 220 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::AND;
	 
#line 348 "SteelScanner.cpp"

                }
                break;

                case 18:
                {

#line 225 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::OR;
	 
#line 360 "SteelScanner.cpp"

                }
                break;

                case 19:
                {

#line 230 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::NOT;
	
#line 372 "SteelScanner.cpp"

                }
                break;

                case 20:
                {

#line 235 "steel.reflex"

		*token =new AstKeyword(m_line,m_script_name); return SteelParser::Token::WHILE;
	
#line 384 "SteelScanner.cpp"

                }
                break;

                case 21:
                {

#line 240 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::LOOP;
	
#line 396 "SteelScanner.cpp"

                }
                break;

                case 22:
                {

#line 245 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::TIMES;
	
#line 408 "SteelScanner.cpp"

                }
                break;

                case 23:
                {

#line 250 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::USING;
	
#line 420 "SteelScanner.cpp"

                }
                break;

                case 24:
                {

#line 255 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::BREAK;
	
#line 432 "SteelScanner.cpp"

                }
                break;

                case 25:
                {

#line 260 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::CONTINUE;
	
#line 444 "SteelScanner.cpp"

                }
                break;

                case 26:
                {

#line 265 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::IF;
	
#line 456 "SteelScanner.cpp"

                }
                break;

                case 27:
                {

#line 270 "steel.reflex"

		*token = new AstKeyword(m_line,m_script_name); return SteelParser::Token::ELSE;
	
#line 468 "SteelScanner.cpp"

                }
                break;

                case 28:
                {

#line 275 "steel.reflex"

		 *token = new AstKeyword(m_line,m_script_name); 
		return SteelParser::Token::RETURN;
	
#line 481 "SteelScanner.cpp"

                }
                break;

                case 29:
                {

#line 281 "steel.reflex"
 
		*token = new AstKeyword(m_line,m_script_name); 
		return SteelParser::Token::FUNCTION;
	
#line 494 "SteelScanner.cpp"

                }
                break;

                case 30:
                {

#line 287 "steel.reflex"

	 	*token = new AstKeyword(m_line,m_script_name); 
		
		return SteelParser::Token::VAR;
	
#line 508 "SteelScanner.cpp"

                }
                break;

                case 31:
                {

#line 294 "steel.reflex"

		*token = new AstFuncIdentifier(m_line,m_script_name,accepted_text);
		return SteelParser::Token::FUNC_IDENTIFIER;
	
#line 521 "SteelScanner.cpp"

                }
                break;

                case 32:
                {

#line 305 "steel.reflex"

		// First, add the accepted_text to the string..
		AstString *pString = (AstString*)*token;		
		pString->addChar(accepted_text[0]);
	
#line 535 "SteelScanner.cpp"

                }
                break;

                case 33:
                {

#line 313 "steel.reflex"

		AstString *pString = (AstString*)*token;	
		pString->addChar(accepted_text[1]);
	
#line 548 "SteelScanner.cpp"

                }
                break;

                case 34:
                {

#line 319 "steel.reflex"
 
		TransitionToState(State::MAIN);
		return SteelParser::Token::STRING; 
	
#line 561 "SteelScanner.cpp"

                }
                break;

                case 35:
                {

#line 326 "steel.reflex"

		//EmitError(GetFileLocation(),"unterminated string literal");
	
#line 573 "SteelScanner.cpp"

                }
                break;


                default: assert(false && "this should never happen"); break;
            }
            just_accepted = true;
        }
        // if the accumulated input buffer isn't empty, that means whatever
        // the first atom inside it went unmatched, so just throw it away,
        // moving the rest back into the input buffer.
        else if (!m_accumulated_input.empty())
        {
            InputAtom unmatched_character = m_accumulated_input[0];
            m_input_buffer.insert(0, m_accumulated_input.c_str()+1);
            m_accumulated_input.clear();
            HandleUnmatchedCharacter(unmatched_character);
        }
        // otherwise, it was the first atom in the input buffer which went
        // unmatched, so dump it.
        else
        {
            InputAtom unmatched_character = m_input_buffer[0];
            m_input_buffer.erase(0, 1);
            HandleUnmatchedCharacter(unmatched_character);
        }
    }
    while (!m_is_end_of_input || just_accepted);


#line 22 "steel.reflex"

			

#line 609 "SteelScanner.cpp"
}

void SteelScanner::PerformStateMachineConsistencyCheck ()
{
    // if any of these assertions fail, the state and/or
    // transition tables were created incorrectly.

    assert(ms_state != NULL && "must have a state table");
    assert(ms_state_count > 0 && "must have at least one state");
    assert(ms_transition != NULL && "must have a transition table");

    {
        Transition const *transition = ms_transition;
        for (State const *state = ms_state, *state_end = ms_state + ms_state_count;
            state != state_end;
            ++state)
        {
            assert(state->m_transition == transition && "states' transitions must be contiguous and in ascending order");
            transition += state->m_transition_count;
        }
        assert(transition == ms_transition + ms_transition_count && "there are too many or too few referenced transitions in the state table");
    }

    for (Transition const *transition = ms_transition, *transition_end = ms_transition + ms_transition_count;
         transition != transition_end;
         ++transition)
    {
        assert(transition->m_type >= Transition::TT_INPUT_ATOM && transition->m_type < Transition::TT_COUNT && "transition Type out of range");
        assert(transition->m_target_state >= ms_state && transition->m_target_state < ms_state + ms_state_count && "transition target state out of range (does not point to a valid state)");
    }
}

bool SteelScanner::IsConditionMet (Transition::Type transition_condition)
{
    // implementation-specific way to get the values of conditionals
    // as set in SteelScanner::RetrieveNextInputAtom().
    switch (transition_condition)
    {
        case Transition::TT_BEGINNING_OF_INPUT:     return m_is_beginning_of_input;
        case Transition::TT_NOT_BEGINNING_OF_INPUT: return !m_is_beginning_of_input;
        case Transition::TT_END_OF_INPUT:           return m_is_end_of_input;
        case Transition::TT_NOT_END_OF_INPUT:       return !m_is_end_of_input;
        case Transition::TT_BEGINNING_OF_LINE:      return m_is_beginning_of_line;
        case Transition::TT_NOT_BEGINNING_OF_LINE:  return !m_is_beginning_of_line;
        case Transition::TT_END_OF_LINE:            return m_is_end_of_line;
        case Transition::TT_NOT_END_OF_LINE:        return !m_is_end_of_line;
        case Transition::TT_WORD_BOUNDARY:          return m_is_word_boundary;
        case Transition::TT_NOT_WORD_BOUNDARY:      return !m_is_word_boundary;
        default: assert(false && "this should never happen"); return false;
    }
}

bool SteelScanner::IsInputAtEnd ()
{

#line 108 "steel.reflex"


	return (m_pBuffer[m_char_cursor] == '\0');

#line 670 "SteelScanner.cpp"
}

SteelScanner::InputAtom SteelScanner::ReadFromInput ()
{

#line 99 "steel.reflex"


	if( m_pBuffer[m_char_cursor] == '\0')
		return '\n';
	else
		return m_pBuffer[m_char_cursor++];


#line 685 "SteelScanner.cpp"
}

void SteelScanner::RetrieveNextInputAtom ()
{
    m_is_beginning_of_input = !m_has_past_beginning_of_input;

    if (!m_is_beginning_of_input && !m_is_end_of_input)
        m_previous_input_atom = m_current_input_atom;

    if (m_input_buffer.empty())
    {
        m_current_input_atom = ReadFromInput();
        m_is_end_of_input = IsInputAtEnd();
        if (!m_is_end_of_input)
            m_input_buffer += m_current_input_atom;
    }
    else
    {
        m_current_input_atom = m_input_buffer[0];
        m_is_end_of_input = IsInputAtEnd();
    }

    DEBUG_SPEW(DEBUG_INPUT_BUFFER, std::cerr << "input buffer = \"" << m_input_buffer << "\"" << std::endl);

    m_is_beginning_of_line = m_previous_input_atom == '\n';
    m_is_end_of_line = m_is_end_of_input || m_current_input_atom == '\n';
    m_is_word_boundary =
        (m_is_beginning_of_line || !isalnum(m_previous_input_atom)) && isalnum(m_current_input_atom)
        ||
        isalnum(m_previous_input_atom) && (!isalnum(m_current_input_atom) || m_is_end_of_line);

    m_has_past_beginning_of_input = true;
}

void SteelScanner::HandleUnmatchedCharacter (InputAtom const unmatched_character)
{

#line 91 "steel.reflex"

	std::cerr << "Warning: unmatched input found." << std::endl;

#line 727 "SteelScanner.cpp"

}

void SteelScanner::TransitionToState (State::Name scanner_state_initial_index)
{
    assert(m_current_state_set.empty() && "you may only call this method during text acceptance");
    assert(scanner_state_initial_index >= 0);
    assert((unsigned int)scanner_state_initial_index < ms_state_count);
    m_initial_state_index = scanner_state_initial_index;
    DEBUG_SPEW(DEBUG_TRANSITION_TO_STATE, std::cerr << "transitioned to state with initial index " << m_initial_state_index << std::endl);
}

void SteelScanner::ResetForNewInput ()
{
    m_has_past_beginning_of_input = false;
    m_input_buffer.clear();
    m_accumulated_input.clear();
    m_previous_input_atom = '\n';
    m_initial_state_index = State::MAIN;
    m_current_state_set.clear();
    m_most_recent_accept_state_set.clear();
    m_most_recent_accept_input_length = 0;

    {

#line 87 "steel.reflex"

	m_line = 0;	

#line 757 "SteelScanner.cpp"
    }
}

void SteelScanner::PerformClosureOfCurrentStateSet ()
{
    StateSet new_state_set;
    for (StateSet::iterator it = m_current_state_set.begin(),
                            it_end = m_current_state_set.end();
         it != it_end;
         ++it)
    {
        State const *state = *it;
        assert(state != NULL);
        PerformClosureOfState(*state, &new_state_set);
    }
    m_current_state_set = new_state_set;
}

void SteelScanner::PerformClosureOfState (State const &state, StateSet *const new_state_set)
{
    assert(new_state_set != NULL);

    bool add_to_current_state_set = false;
    for (Transition const *transition = state.m_transition,
                          *transition_end = transition + state.m_transition_count;
         transition != transition_end;
         ++transition)
    {
        if (transition->m_type == Transition::TT_EPSILON || transition->IsConditional() && IsConditionMet(transition->m_type))
            PerformClosureOfState(*transition->m_target_state, new_state_set);
        else
            add_to_current_state_set = true;
    }
    if (add_to_current_state_set || state.IsAcceptState())
        new_state_set->insert(&state);
}

void SteelScanner::ProcessInput ()
{
    if (m_is_end_of_input)
    {
        m_current_state_set.clear();
        return;
    }

    assert(!m_input_buffer.empty());

    StateSet new_state_set;
    for (StateSet::iterator it = m_current_state_set.begin(),
                            it_end = m_current_state_set.end();
         it != it_end;
         ++it)
    {
        State const *state = *it;
        assert(state != NULL);
        for (Transition const *transition = state->m_transition,
                              *transition_end = transition + state->m_transition_count;
             transition != transition_end;
             ++transition)
        {
            if (*transition == m_input_buffer[0])
                new_state_set.insert(transition->m_target_state);
        }
    }
    m_current_state_set = new_state_set;
}

bool SteelScanner::AnyStatesInTheCurrentStateSetAreAcceptStates () const
{
    for (StateSet::const_iterator it = m_current_state_set.begin(),
                                  it_end = m_current_state_set.end();
         it != it_end;
         ++it)
    {
        State const *state = *it;
        assert(state != NULL);
        if (state->IsAcceptState())
            return true;
    }
    return false;
}

void SteelScanner::UpdateMostRecentAcceptStateSet ()
{
    StateSet new_state_set;
    for (StateSet::iterator it = m_current_state_set.begin(),
                          it_end = m_current_state_set.end();
         it != it_end;
         ++it)
    {
        State const *state = *it;
        assert(state != NULL);
        if (state->IsAcceptState())
            new_state_set.insert(state);
    }
    m_most_recent_accept_state_set = new_state_set;
    m_most_recent_accept_input_length = m_accumulated_input.length();
    DEBUG_SPEW(DEBUG_MOST_RECENT_ACCEPT_STATE_SET, PrintStateSet(std::cerr, "most recent accept state set", m_most_recent_accept_state_set))
}

void SteelScanner::PrintStateSet (std::ostream &stream, std::string const &message, StateSet const &state_set)
{
    stream << "StateSet: \"" << message << "\" - ";
    for (StateSet::const_iterator it = state_set.begin(),
                               it_end = state_set.end();
         it != it_end;
         ++it)
    {
        State const *state = *it;
        assert(state != NULL);
        stream << (state->IsAcceptState() ? "*" : "") << state - ms_state << ' ';
    }
    stream << std::endl;
}

// the order of the states indicates priority (only for accept states).
// the lower the state's index in this array, the higher its priority.
SteelScanner::State const SteelScanner::ms_state[] =
{
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 0, ms_transition+0 },
    { 52, ms_transition+0 },
    { 3, ms_transition+52 },
    { 1, ms_transition+55 },
    { 2, ms_transition+56 },
    { 1, ms_transition+58 },
    { 3, ms_transition+59 },
    { 4, ms_transition+62 },
    { 1, ms_transition+66 },
    { 2, ms_transition+67 },
    { 1, ms_transition+69 },
    { 2, ms_transition+70 },
    { 3, ms_transition+72 },
    { 1, ms_transition+75 },
    { 5, ms_transition+76 },
    { 3, ms_transition+81 },
    { 1, ms_transition+84 },
    { 5, ms_transition+85 },
    { 1, ms_transition+90 },
    { 1, ms_transition+91 },
    { 1, ms_transition+92 },
    { 1, ms_transition+93 },
    { 1, ms_transition+94 },
    { 1, ms_transition+95 },
    { 1, ms_transition+96 },
    { 1, ms_transition+97 },
    { 1, ms_transition+98 },
    { 1, ms_transition+99 },
    { 1, ms_transition+100 },
    { 1, ms_transition+101 },
    { 1, ms_transition+102 },
    { 1, ms_transition+103 },
    { 1, ms_transition+104 },
    { 1, ms_transition+105 },
    { 1, ms_transition+106 },
    { 1, ms_transition+107 },
    { 1, ms_transition+108 },
    { 1, ms_transition+109 },
    { 1, ms_transition+110 },
    { 1, ms_transition+111 },
    { 1, ms_transition+112 },
    { 1, ms_transition+113 },
    { 1, ms_transition+114 },
    { 1, ms_transition+115 },
    { 1, ms_transition+116 },
    { 1, ms_transition+117 },
    { 1, ms_transition+118 },
    { 1, ms_transition+119 },
    { 1, ms_transition+120 },
    { 1, ms_transition+121 },
    { 1, ms_transition+122 },
    { 1, ms_transition+123 },
    { 1, ms_transition+124 },
    { 1, ms_transition+125 },
    { 1, ms_transition+126 },
    { 1, ms_transition+127 },
    { 1, ms_transition+128 },
    { 1, ms_transition+129 },
    { 1, ms_transition+130 },
    { 1, ms_transition+131 },
    { 1, ms_transition+132 },
    { 1, ms_transition+133 },
    { 1, ms_transition+134 },
    { 1, ms_transition+135 },
    { 1, ms_transition+136 },
    { 1, ms_transition+137 },
    { 1, ms_transition+138 },
    { 1, ms_transition+139 },
    { 1, ms_transition+140 },
    { 1, ms_transition+141 },
    { 1, ms_transition+142 },
    { 1, ms_transition+143 },
    { 5, ms_transition+144 },
    { 7, ms_transition+149 },
    { 2, ms_transition+156 },
    { 1, ms_transition+158 }
};
unsigned int const SteelScanner::ms_state_count = sizeof(SteelScanner::ms_state) / sizeof(SteelScanner::State);

SteelScanner::Transition const SteelScanner::ms_transition[] =
{
    { Transition::TT_END_OF_INPUT, 0, 0, ms_state+0 },
    { Transition::TT_INPUT_ATOM, 10, 10, ms_state+1 },
    { Transition::TT_INPUT_ATOM, 9, 9, ms_state+37 },
    { Transition::TT_INPUT_ATOM, 32, 32, ms_state+37 },
    { Transition::TT_INPUT_ATOM, 34, 34, ms_state+3 },
    { Transition::TT_INPUT_ATOM, 48, 48, ms_state+4 },
    { Transition::TT_INPUT_ATOM_RANGE, 49, 57, ms_state+38 },
    { Transition::TT_INPUT_ATOM, 48, 48, ms_state+40 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+44 },
    { Transition::TT_INPUT_ATOM, 36, 36, ms_state+47 },
    { Transition::TT_INPUT_ATOM, 64, 64, ms_state+50 },
    { Transition::TT_INPUT_ATOM, 59, 59, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 64, 64, ms_state+9 },
    { Transition::TT_END_OF_LINE, 0, 0, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 40, 40, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 41, 41, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 44, 44, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 37, 37, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 43, 43, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 45, 45, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 94, 94, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 61, 61, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 91, 91, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 93, 93, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 123, 123, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 125, 125, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 37, 37, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 47, 47, ms_state+9 },
    { Transition::TT_INPUT_ATOM, 60, 60, ms_state+53 },
    { Transition::TT_INPUT_ATOM, 62, 62, ms_state+54 },
    { Transition::TT_INPUT_ATOM, 33, 33, ms_state+55 },
    { Transition::TT_INPUT_ATOM, 61, 61, ms_state+56 },
    { Transition::TT_INPUT_ATOM, 60, 60, ms_state+14 },
    { Transition::TT_INPUT_ATOM, 62, 62, ms_state+15 },
    { Transition::TT_INPUT_ATOM, 100, 100, ms_state+16 },
    { Transition::TT_INPUT_ATOM, 97, 97, ms_state+57 },
    { Transition::TT_INPUT_ATOM, 111, 111, ms_state+59 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+60 },
    { Transition::TT_INPUT_ATOM, 119, 119, ms_state+62 },
    { Transition::TT_INPUT_ATOM, 108, 108, ms_state+66 },
    { Transition::TT_INPUT_ATOM, 116, 116, ms_state+69 },
    { Transition::TT_INPUT_ATOM, 117, 117, ms_state+73 },
    { Transition::TT_INPUT_ATOM, 98, 98, ms_state+77 },
    { Transition::TT_INPUT_ATOM, 99, 99, ms_state+81 },
    { Transition::TT_INPUT_ATOM, 105, 105, ms_state+88 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+89 },
    { Transition::TT_INPUT_ATOM, 114, 114, ms_state+92 },
    { Transition::TT_INPUT_ATOM, 102, 102, ms_state+97 },
    { Transition::TT_INPUT_ATOM, 118, 118, ms_state+104 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 90, ms_state+106 },
    { Transition::TT_INPUT_ATOM, 95, 95, ms_state+106 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 122, ms_state+106 },
    { Transition::TT_INPUT_ATOM, 9, 9, ms_state+37 },
    { Transition::TT_INPUT_ATOM, 32, 32, ms_state+37 },
    { Transition::TT_EPSILON, 0, 0, ms_state+2 },
    { Transition::TT_EPSILON, 0, 0, ms_state+39 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+39 },
    { Transition::TT_EPSILON, 0, 0, ms_state+4 },
    { Transition::TT_INPUT_ATOM, 120, 120, ms_state+41 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+42 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 70, ms_state+42 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 102, ms_state+42 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+42 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 70, ms_state+42 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 102, ms_state+42 },
    { Transition::TT_EPSILON, 0, 0, ms_state+5 },
    { Transition::TT_INPUT_ATOM, 46, 46, ms_state+45 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+44 },
    { Transition::TT_EPSILON, 0, 0, ms_state+43 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+46 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+46 },
    { Transition::TT_EPSILON, 0, 0, ms_state+6 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 90, ms_state+48 },
    { Transition::TT_INPUT_ATOM, 95, 95, ms_state+48 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 122, ms_state+48 },
    { Transition::TT_EPSILON, 0, 0, ms_state+49 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+49 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 90, ms_state+49 },
    { Transition::TT_INPUT_ATOM, 95, 95, ms_state+49 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 122, ms_state+49 },
    { Transition::TT_EPSILON, 0, 0, ms_state+7 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 90, ms_state+51 },
    { Transition::TT_INPUT_ATOM, 95, 95, ms_state+51 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 122, ms_state+51 },
    { Transition::TT_EPSILON, 0, 0, ms_state+52 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+52 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 90, ms_state+52 },
    { Transition::TT_INPUT_ATOM, 95, 95, ms_state+52 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 122, ms_state+52 },
    { Transition::TT_EPSILON, 0, 0, ms_state+8 },
    { Transition::TT_INPUT_ATOM, 61, 61, ms_state+10 },
    { Transition::TT_INPUT_ATOM, 61, 61, ms_state+11 },
    { Transition::TT_INPUT_ATOM, 61, 61, ms_state+12 },
    { Transition::TT_INPUT_ATOM, 61, 61, ms_state+13 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+58 },
    { Transition::TT_INPUT_ATOM, 100, 100, ms_state+17 },
    { Transition::TT_INPUT_ATOM, 114, 114, ms_state+18 },
    { Transition::TT_INPUT_ATOM, 111, 111, ms_state+61 },
    { Transition::TT_INPUT_ATOM, 116, 116, ms_state+19 },
    { Transition::TT_INPUT_ATOM, 104, 104, ms_state+63 },
    { Transition::TT_INPUT_ATOM, 105, 105, ms_state+64 },
    { Transition::TT_INPUT_ATOM, 108, 108, ms_state+65 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+20 },
    { Transition::TT_INPUT_ATOM, 111, 111, ms_state+67 },
    { Transition::TT_INPUT_ATOM, 111, 111, ms_state+68 },
    { Transition::TT_INPUT_ATOM, 112, 112, ms_state+21 },
    { Transition::TT_INPUT_ATOM, 105, 105, ms_state+70 },
    { Transition::TT_INPUT_ATOM, 109, 109, ms_state+71 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+72 },
    { Transition::TT_INPUT_ATOM, 115, 115, ms_state+22 },
    { Transition::TT_INPUT_ATOM, 115, 115, ms_state+74 },
    { Transition::TT_INPUT_ATOM, 105, 105, ms_state+75 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+76 },
    { Transition::TT_INPUT_ATOM, 103, 103, ms_state+23 },
    { Transition::TT_INPUT_ATOM, 114, 114, ms_state+78 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+79 },
    { Transition::TT_INPUT_ATOM, 97, 97, ms_state+80 },
    { Transition::TT_INPUT_ATOM, 107, 107, ms_state+24 },
    { Transition::TT_INPUT_ATOM, 111, 111, ms_state+82 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+83 },
    { Transition::TT_INPUT_ATOM, 116, 116, ms_state+84 },
    { Transition::TT_INPUT_ATOM, 105, 105, ms_state+85 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+86 },
    { Transition::TT_INPUT_ATOM, 117, 117, ms_state+87 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+25 },
    { Transition::TT_INPUT_ATOM, 102, 102, ms_state+26 },
    { Transition::TT_INPUT_ATOM, 108, 108, ms_state+90 },
    { Transition::TT_INPUT_ATOM, 115, 115, ms_state+91 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+27 },
    { Transition::TT_INPUT_ATOM, 101, 101, ms_state+93 },
    { Transition::TT_INPUT_ATOM, 116, 116, ms_state+94 },
    { Transition::TT_INPUT_ATOM, 117, 117, ms_state+95 },
    { Transition::TT_INPUT_ATOM, 114, 114, ms_state+96 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+28 },
    { Transition::TT_INPUT_ATOM, 117, 117, ms_state+98 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+99 },
    { Transition::TT_INPUT_ATOM, 99, 99, ms_state+100 },
    { Transition::TT_INPUT_ATOM, 116, 116, ms_state+101 },
    { Transition::TT_INPUT_ATOM, 105, 105, ms_state+102 },
    { Transition::TT_INPUT_ATOM, 111, 111, ms_state+103 },
    { Transition::TT_INPUT_ATOM, 110, 110, ms_state+29 },
    { Transition::TT_INPUT_ATOM, 97, 97, ms_state+105 },
    { Transition::TT_INPUT_ATOM, 114, 114, ms_state+30 },
    { Transition::TT_EPSILON, 0, 0, ms_state+107 },
    { Transition::TT_INPUT_ATOM_RANGE, 48, 57, ms_state+107 },
    { Transition::TT_INPUT_ATOM_RANGE, 65, 90, ms_state+107 },
    { Transition::TT_INPUT_ATOM, 95, 95, ms_state+107 },
    { Transition::TT_INPUT_ATOM_RANGE, 97, 122, ms_state+107 },
    { Transition::TT_EPSILON, 0, 0, ms_state+31 },
    { Transition::TT_INPUT_ATOM_RANGE, 0, 33, ms_state+32 },
    { Transition::TT_INPUT_ATOM_RANGE, 35, 91, ms_state+32 },
    { Transition::TT_INPUT_ATOM_RANGE, 93, 255, ms_state+32 },
    { Transition::TT_INPUT_ATOM, 92, 92, ms_state+109 },
    { Transition::TT_INPUT_ATOM, 34, 34, ms_state+34 },
    { Transition::TT_EPSILON, 0, 0, ms_state+110 },
    { Transition::TT_INPUT_ATOM, 92, 92, ms_state+110 },
    { Transition::TT_INPUT_ATOM_RANGE, 0, 9, ms_state+33 },
    { Transition::TT_INPUT_ATOM_RANGE, 11, 255, ms_state+33 },
    { Transition::TT_END_OF_INPUT, 0, 0, ms_state+35 }
};
unsigned int const SteelScanner::ms_transition_count = sizeof(SteelScanner::ms_transition) / sizeof(SteelScanner::Transition);


#line 43 "steel.reflex"

void SteelScanner::setBuffer(const char * pBuffer, const std::string &name)
{
	assert ( NULL != pBuffer );
	m_char_cursor = 0;
	m_line = 1;
	m_pBuffer = pBuffer;
	m_script_name = name;
}
int SteelScanner::ToInt(const std::string &text)
{
	std::istringstream str(text);
	int i;
	str >> i;

	return i;
}

double SteelScanner::ToFloat(const std::string &text)
{
	std::istringstream str(text);
	double d;
	str >> d;

	return d;
}

int SteelScanner::ToIntFromHex(const std::string &text)
{
	std::istringstream str(text);
	int i;

	str >> std::hex >> i;
}


#line 1193 "SteelScanner.cpp"
