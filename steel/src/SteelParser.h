// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY
// SteelParser.h generated by trison
// from steel.trison using trison.cpp.targetspec and trison.cpp.header.codespec
// DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY ! DO NOT MODIFY

#include <cassert>
#include <deque>
#include <iostream>

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


#line 5 "steel.trison"

#if !defined(STEEL_PARSER_HPP_)
#define STEEL_PARSER_HPP_

	#include <cassert>
	#include <string>
	#include <cstring>
	#include "File.h"
	#include "Ast.h"
	namespace Steel { 
	class SteelScanner;


#line 103 "SteelParser.h"

/** A parser class generated by trison
  * from steel.trison using trison.cpp.targetspec and trison.cpp.header.codespec.
  *
  * The term "primary source" will be used to refer to the .trison source file from which
  * this file was generated.  Specifically, the primary source is steel.trison.
  *
  * The term "client" will be used to refer to the programmer who is writing the trison
  * primary source file to generate a parser (e.g. "the client shouldn't return X from Y"
  * or "the client must provide a way to X and Y").
  *
  * @brief A parser class.
  */
class SteelParser
{
public:

    /// Return values for Parse().
    enum ParserReturnCode
    {
        /// Indicates the Parse() method returned successfully.
        PRC_SUCCESS = 0,
        /// Indicates an unhandled parse error occurred (i.e. no %error-accepting
        /// rules were encountered).
        PRC_UNHANDLED_PARSE_ERROR = 1
    }; // end of enum SteelParser::ParserReturnCode

    /// "Namespace" for SteelParser::Terminal::Name, which enumerates all valid
    /// token ids which this parser will accept as lookaheads.
    struct Terminal
    {
        /** There are two special terminals: END_ and ERROR_.
          *
          * SteelParser::Terminal::END_ should be returned in %target.cpp.scan_actions
          * by the client when the input source has reached the end of input.  The parser
          * will not request any more input after SteelParser::Terminal::END_ is received.
          *
          * SteelParser::Terminal::ERROR_ should not ever be used by the client, as
          * it is used internally by the parser.
          *
          * The rest are the terminals as declared in the primary source, and should
          * be used by the client when returning from %target.cpp.scan_actions.
          *
          * @brief Acceptable values returnable to the parser in %target.cpp.scan_actions.
          */
        enum Name
        {
            END_ = 256,
            ERROR_ = 257,
            D = 258,
            BINOP_COMPARE = 259,
            AND = 260,
            OR = 261,
            NOT = 262,
            WHILE = 263,
            BREAK = 264,
            CONTINUE = 265,
            RETURN = 266,
            IF = 267,
            ELSE = 268,
            FUNCTION = 269,
            IDENTIFIER = 270,
            FOR = 271,
            FOREACH = 272,
            WITHIN = 273,
            INCLUDE = 274,
            VAR = 275,
            STRING = 276,
            BINOP_MULT_DIV_MOD = 277,
            BINOP_ASSIGNMENT = 278,
            LITERAL = 279,
            INCREMENT = 280,
            DECREMENT = 281,
            POP = 282,
            POPB = 283,
            PUSH = 284,
            PUSHB = 285,
            REMOVE = 286,
            CONSTANT = 287,
            DO = 288,
            IMPORT = 289,
            SCOPE = 290,
            SWITCH = 291,
            CASE = 292,
            DEFAULT = 293,
            CASE_DELIM = 294,
            LAMBDA = 295,
            MAKE_PAIR = 296,
            BIN_AND = 297,
            BIN_OR = 298,
            BIN_NOT = 299
        }; // end of enum SteelParser::Terminal::Name
    }; // end of struct SteelParser::Terminal

    /// "Namespace" for SteelParser::ParseNonterminal::Name, which enumerates
    /// all valid nonterminals this parser can recognize.
    struct ParseNonterminal
    {
        /** The Parse() method doesn't necessarily have to start with the
          * value of the %default_parse_nonterminal directive; it can
          * attempt to parse any of the nonterminals defined in the primary
          * source file.  These enums are the way to specify which nonterminal
          * to parse.
          *
          * @brief Acceptable nonterminals recognizable by this parser.
          */
        enum Name
        {
            array_literal = 291,
            call = 308,
            case_list = 304,
            exp = 306,
            func_definition = 285,
            optionalexp = 300,
            pair = 296,
            pair_list = 294,
            param_definition = 287,
            param_list = 313,
            root = 0,
            statement = 298,
            statement_list = 289,
            value = 302,
            vardecl = 311,
            /// Nonterminal which will be attempted to be parsed by the Parse()
            /// method by default (specified by the %default_parse_nonterminal
            /// directive).
            default_ = root
        }; // end of enum SteelParser::ParseNonterminal::Name
    }; // end of struct SteelParser::ParseNonterminal

    /** The client should package-up and return a SteelParser::Token from
      * the code specified by %target.cpp.scan_actions, which delivers the
      * token type and token data to the parser for input.  The constructor
      * takes one or two parameters; the second can be omitted, indicating
      * that the %target.cpp.token_data_default value will be used.
      *
      * @brief Return type for %target.cpp.scan_actions.
      */
    struct Token
    {
        typedef BarfCpp_::Uint32 Id; // TODO -- smallest int
        typedef AstBase* Data;

        Id m_id;
        Data m_data;

        /** @param id Gives the token id, e.g. Terminal::END_ or whatever
          *        other terminals were declared in the primary source.
          * @param data Gives the data associated with this token, e.g. if
          *        you were constructing an AST, data would point to an AST
          *        node constructed during scanning.
          *
          * @brief Constructor for Token struct.
          */
        Token (Id id, Data const &data = NULL)
            :
            m_id(id),
            m_data(data)
        {
            assert(m_id != Nonterminal_::none_ &&
                   m_id != Terminal::ERROR_ &&
                   "do not construct a token with this id");
        }
    }; // end of struct SteelParser::Token

public:

    /// Constructor.  The client can specify parameters in the primary source
    /// via the %target.cpp.constructor_parameters directive.
    SteelParser ();
    /// Destructor.  The client can force the destructor to be declared virtual
    /// by specifying the %target.cpp.force_virtual_destructor directive in the
    /// primary source.
    ~SteelParser ();

    /** It is not sufficient to just check the EOF condition on the input
      * source (e.g. the scanner, cin, etc), because the parser may have read,
      * but not consumed, additional lookaheads up to EOF.  Thus checking
      * the input source for EOF condition may give false positives.  This
      * method should be the preferred means to check EOF condition.
      *
      * It should be noted that this may cause the parser to read (but never
      * consume) up to one additional lookahead token, owing to the necessity
      * of checking what the next lookahead token is.
      *
      * @brief Returns true if and only if the next unshifted lookahead
      *        token is Terminal::END_.
      */
    bool IsAtEndOfInput ();

    /// Returns true if and only if "debug spew" is enabled (which prints, to
    /// std::cerr, exactly what the parser is doing at each step).  This method,
    /// along with all other debug spew code can be removed by removing the
    /// %target.cpp.generate_debug_spew_code directive from the primary source.
    bool DebugSpew () const { return m_debug_spew_; }
    /// Sets the debug spew flag (see DebugSpew()).  This method,
    /// along with all other debug spew code can be removed by removing the
    /// %target.cpp.generate_debug_spew_code directive from the primary source.
    void DebugSpew (bool debug_spew) { m_debug_spew_ = debug_spew; }

    /** This parser is capable of attempting multiple contiguous parses from the
      * same input source.  The lookahead queue is preserved between calls to
      * Parse().  Therefore, if the input source changes, the lookahead queue
      * must be cleared so that the new input source can be read.  The client
      * must call this method if the input source changes.
      *
      * @brief This method must be called if the input source changes.
      */
    void ResetForNewInput ();

    /** The %target.cpp.parse_method_access directive can be used to specify the
      * access level of this method.
      *
      * The %target.cpp.top_of_parse_method_actions and
      * %target.cpp.bottom_of_parse_method_actions directives can be used to specify
      * code to execute at the beginning and end, respectively, of the Parse() method.
      * This includes the ability to enclose the body of the Parse() method within a
      * try {} block, for exception handling (if exceptions are thrown in scan_actions
      * or any reduction rule code, then the %target.cpp.enable_scan_actions_exceptions
      * or %target.cpp.enable_reduction_rule_exceptions directives must be specified
      * respectively; this will cause the parser to catch and rethrow any exceptions
      * thrown by scan_actions or reduction rule code, allowing it to clean up
      * dynamically allocated memory, etc.
      *
      * @param return_token A pointer to the value which will be assigned to upon
      *        successfully parsing the requested nonterminal. If the parse fails,
      *        the value of the %target.cpp.token_data_default directive will
      *        be assigned.
      * @param nonterminal_to_parse The Parse() method can attempt to parse any
      *        nonterminal declared in the primary source.  If unspecified, the
      *        Parse() method will attempt to parse the nonterminal specified by the
      *        %default_parse_nonterminal directive.
      * @return SteelParser::PRC_SUCCESS if the parse was successful (which includes
      *         occurrences of parse errors which were handled by client-specified
      *         %error-accepting rules), or SteelParser::PRC_UNHANDLED_PARSE_ERROR
      *         if a parse error was not handled by any %error-accepting rules.
      *
      * @brief This is the main method of the parser; it will attempt to parse
      *        the nonterminal specified.
      */
    ParserReturnCode Parse (AstBase* *return_token, ParseNonterminal::Name nonterminal_to_parse = ParseNonterminal::root);


#line 42 "steel.trison"

  	void setBuffer(const char *pBuffer, const std::string &script_name);
	void setFileProvider(IFileProvider* provider);
	void append(const std::string& str);
	bool hadError() const { return mbErrorEncountered; }
	std::string getErrors() const { return mErrors; }
    	Token Scan ();
    	void SetScannerDebugSpew(bool on);
private:
	void addError(unsigned int line, const std::string &error);
	SteelScanner *m_scanner;
    	const char *m_pBuffer;
	bool mbErrorEncountered;
	FileProvider m_default_file_provider;
	IFileProvider* m_file_provider;
	std::string mErrors;

#line 365 "SteelParser.h"


private:

    // ///////////////////////////////////////////////////////////////////////
    // begin internal trison-generated parser guts -- don't use
    // ///////////////////////////////////////////////////////////////////////

    struct Nonterminal_
    {
        enum Name
        {
            none_ = 0,
            root = 300,
            func_definition = 301,
            param_definition = 302,
            statement_list = 303,
            array_literal = 304,
            pair_list = 305,
            pair = 306,
            statement = 307,
            optionalexp = 308,
            value = 309,
            case_list = 310,
            exp = 311,
            call = 312,
            vardecl = 313,
            param_list = 314
        }; // end of enum SteelParser::Nonterminal_::Name
    }; // end of struct SteelParser::Nonterminal_
    struct Transition_;

    struct StackElement_
    {
        BarfCpp_::Uint32 m_state_index;
        Token::Data m_token_data;

        StackElement_ ()
            :
            m_state_index(BarfCpp_::Uint32(-1)),
            m_token_data(NULL)
        { }
        StackElement_ (BarfCpp_::Uint32 state_index, Token::Data token_data)
            :
            m_state_index(state_index),
            m_token_data(token_data)
        { }
    }; // end of struct SteelParser::StackElement_

    typedef std::deque<StackElement_> Stack_;
    typedef std::deque<Token> LookaheadQueue_;

    ParserReturnCode Parse_ (AstBase* *return_token, ParseNonterminal::Name nonterminal_to_parse);
    void ThrowAwayToken_ (Token::Data &token_data) throw();
    void ResetForNewInput_ () throw();
    Token Scan_ () throw();
    void ClearStack_ () throw();
    void ClearLookaheadQueue_ () throw();
    Token const &Lookahead_ (LookaheadQueue_::size_type index) throw();
    bool ExerciseTransition_ (Transition_ const &transition);
    Token::Data ExecuteReductionRule_ (BarfCpp_::Uint32 const rule_index_) throw();
    // debug spew methods
    void PrintParserStatus_ (std::ostream &stream) const;
    void PrintIndented_ (std::ostream &stream, char const *string) const;

    struct Rule_
    {
        Token::Id m_reduction_nonterminal_token_id;
        BarfCpp_::Uint32 m_token_count;
        char const *m_description;
    }; // end of struct SteelParser::Rule_

    struct State_
    {
        BarfCpp_::Size const m_transition_count; // TODO: smallest int
        Transition_ const *m_transition_table;
        char const *m_description;
    }; // end of struct SteelParser::State_

    struct Transition_
    {
        enum Type { ERROR_PANIC = 0, RETURN, REDUCE, SHIFT };
        BarfCpp_::Uint8 m_type;
        BarfCpp_::Uint32 m_data; // TODO: smallest int
        BarfCpp_::Uint32 m_lookahead_count; // TODO smallest int
        Token::Id const *m_lookahead_sequence;
    }; // end of struct SteelParser::Transition_

    Stack_ m_stack_;
    LookaheadQueue_ m_lookahead_queue_;
    bool m_is_in_error_panic_;
    bool m_debug_spew_;

    static Rule_ const ms_rule_table_[];
    static BarfCpp_::Size const ms_rule_count_;
    static State_ const ms_state_table_[];
    static BarfCpp_::Size const ms_state_count_;
    static Transition_ const ms_transition_table_[];
    static BarfCpp_::Size const ms_transition_count_;
    static Token::Id const ms_lookahead_table_[];
    static BarfCpp_::Size const ms_lookahead_count_;
    static char const *const ms_token_name_table_[];
    static BarfCpp_::Size const ms_token_name_count_;

    friend std::ostream &operator << (std::ostream &stream, SteelParser::Token const &token);

    // ///////////////////////////////////////////////////////////////////////
    // end of internal trison-generated parser guts
    // ///////////////////////////////////////////////////////////////////////
}; // end of class SteelParser

std::ostream &operator << (std::ostream &stream, SteelParser::Token const &token);


#line 20 "steel.trison"

} // namespace Steel
#endif // !defined(STEEL_PARSER_HPP_)

#line 485 "SteelParser.h"
