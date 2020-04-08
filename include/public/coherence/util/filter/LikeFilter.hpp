/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LIKE_FILTER_HPP
#define COH_LIKE_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Filter which compares the result of a method invocation with a value for
* pattern match. A pattern can include regular characters and wildcard
* characters '_' and '%'.
*
* During pattern matching, regular characters must exactly match the
* characters in an evaluated string. Wildcard character '_' (underscore) can
* be matched with any single character, and wildcard character '%' can be
* matched with any string fragment of zero or more characters.
*
* @author djl  2008.07.24
*/
class COH_EXPORT LikeFilter
    : public class_spec<LikeFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<LikeFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        LikeFilter();

        /**
        * Construct a LikeFilter for pattern match.
        *
        * @param vExtractor   the ValueExtractor to use by this filter
        * @param vsPattern    the string pattern to compare the result with
        * @param chEscape     the escape character for escaping '%' and '_'
        * @param fIgnoreCase  true to be case-insensitive
        */
        LikeFilter(ValueExtractor::View vExtractor, String::View vsPattern,
                wchar16_t chEscape = (wchar16_t) 0, bool fIgnoreCase = false);

    private:
        /**
        * Initialize this filter.
        *
        * @param chEscape     the escape character for escaping '%' and '_'
        * @param fIgnoreCase  true to be case-insensitive
        */
        void init(wchar16_t chEscape, bool fIgnoreCase);


    // ----- ExtractorFilter interface --------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluateExtracted(Object::View vExtracted) const;


    // ----- IndexAwareFilter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t calculateEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Filter::View applyIndex(Map::View vMapIndexes,
                Set::Handle hSetKeys) const;


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the filter's pattern string.
        *
        * @return the pattern string
        */
        virtual String::View getPattern() const;

        /**
        * Check whether or not the filter is case incensitive.
        *
        * @return true iff case insensitivity is specifically enabled
        */
        virtual bool isIgnoreCase() const;

        /**
        * Obtain the escape character that is used for escaping '%' and '_'
        * in the pattern or zero if there is no escape.
        *
        * @return the escape character
        */
        virtual wchar16_t getEscapeChar() const;

        /**
        * Display the execution plan that the LikeFilter has selected.
        *
        * @param out  the ostream to display the plan on
        */
        COH_INLINE void showPlan(std::ostream &out) const
            {
            out << formatPlan();
            }

        /**
        * Return the execution plan that the LikeFilter has selected.
        *
        * @return the plan
        */
        virtual String::View formatPlan() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Build a plan for processing the LIKE functionality.
        */
        virtual void buildPlan();

        /**
        * Check the passed String value to see if it matches the pattern that
        * this filter was constructed with.
        *
        * @param vsValue  the String value to match against this filter's
        *                 pattern
        *
        * @return true iff the passed String value is LIKE this filter's
        * pattern
        */
        virtual bool isMatch(String::View vsValue) const;


    // ----- nested class: MatchStep ----------------------------------------

    private:
        /**
        * Handles one matching step for a literal or a
        * character-by-character (literal and/or '_' matching).
        */
        class MatchStep
            : public class_spec<MatchStep>
            {
            friend class factory<MatchStep>;

            // ----- constructor ----------------------------------------

            protected:
                /**
                * Construct a MatchStep object.
                *
                * @param vsMatch        the string of characters to match
                * in this step
                * @param vBitset        corresponding to each character, true
                *                       if any character is allowed ('_')
                * @param fIsIgnoreCase  true if ignoring case
                */
                MatchStep(String::View vsMatch, HashSet::View vBitset,
                        bool fIsIgnoreCase);

            // ----- matching methods -----------------------------------

            public:
                /**
                * Find the first index of this match step in the passed
                * character array starting at the passed offset and
                * within the specified number of characters.
                *
                * @param vach      the array of characters within which
                *                  to find a match
                * @param ofBegin   the starting offset in character
                *                  array<tt>ach</tt> to start looking
                *                  for a match
                * @param ofEnd     the first offset in the character
                *                  array <tt>ach</tt> which is beyond
                *                  the region that this operation is
                *                  allowed to search through to find a
                *                  match
                *
                * @return the first index at which the match is made,
                * or Array<char>::npos if the match cannot be made in the
                * designated range of offsets
                */
                virtual size32_t indexOf(Array<char>::View vach,
                        size32_t ofBegin, size32_t ofEnd) const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * @return the match pattern as a String
                */
                virtual String::View getString() const;

                /**
                * @return the length of the match pattern
                */
                virtual size32_t getLength() const;

                /**
                * @return true if there are no wildcards ('_') in the match
                * pattern
                */
                virtual bool isLiteral() const;

            // ----- Object methods -------------------------------------

            public:
                TypedHandle<const String> toString() const;

            // ----- data members ---------------------------------------

            private:
                /**
                * The match pattern, as a String.
                */
                FinalView<String> f_vsMatch;

                /**
                * The match pattern, as an array of char values. If the
                * filter is case insensitive, then this is the uppercase form
                *  of the char values.
                */
                MemberView<Array<char> > m_vachMatch;
                /**
                * The match pattern for a case insensitive like filter, as an
                * array of lowercase char values. For case sensitive filters,
                * this is null.
                */
                MemberView<Array<char> > m_vachLower;

                /**
                * For each character, true if the character is a wildcard
                * ('_'), or null if there are no wildcards.
                */
                MemberView<Array<bool> > m_vafAny;

                /**
                * Number of leading wildcards.
                */
                size32_t m_cchSkipFront;

                /**
                * Number of trailing wildcards.
                */
                size32_t m_cchSkipBack;

                /**
                * True if there are any wildcards in the middle.
                */
                bool m_fMiddleWilds;

                /**
                 * True if there we are ignoring case.
                 */
                 bool m_fIsIgnoreCase;

                // ----- friends ----------------------------------------

                friend class LikeFilter;
                };


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The escape character for escaping '_' and '%' in the pattern.
        * The valu zero is reserved to mean "no escape".
        */
        wchar16_t m_chEscape;

        /**
        * The option to ignore case sensitivity. True means that the filter
        * will match ignoring case.
        */
        bool m_fIgnoreCase;

        /**
        * Optimization plan number. Zero means default iterative evalution is
        * necessary.
        */
        int32_t m_nPlan;

        /**
        * Used by single-character matching optimization plans.
        */
        char m_chPart;

        /**
        * Used by string-character matching optimization plans.
        */
        MemberView<String>  m_vsPart;

        /**
        * The "front" matching step used by the iterative processing; null if
        * the pattern starts with '%'.
        */
        MemberView<MatchStep> m_vStepFront;

        /**
        * The "back" matching step used by the iterative processing; null if
        * the pattern ends with '%'.
        */
        MemberView<MatchStep> m_vStepBack;

        /**
        * For iterative plans with a NULL "back" matching step, is trailing
        * data permitted.
        */
        bool m_fTrailingTextAllowed;

        /**
        * The array of "middle" matching steps used by the iterative
        * processing; may be NULL if none.
        */
        MemberView<ObjectArray> m_vaStepMiddle;

        /**
        * The flag that indicates whether the pattern is ascii.  This allows
        * the match to execute locally.
        */
        bool m_fPatternIsASCII;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_LIKE_FILTER_HPP
