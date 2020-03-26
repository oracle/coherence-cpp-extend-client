/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BETWEEN_FILTER_HPP
#define COH_BETWEEN_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/SortedMap.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/AndFilter.hpp"
#include "coherence/util/filter/ExtractorFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Filter which compares the result of a method invocation with a value for
* "Between" condition.  We use the standard ISO/IEC 9075:1992 semantic,
* according to which "X between Y and Z" is equivalent to "X >= Y && X <= Z".
* In a case when either result of a method invocation or a value to compare
* are equal to null, the <tt>evaluate</tt> test yields <tt>false</tt>.
* This approach is equivalent to the way the NULL values are handled by SQL.
*
* @author djl  2008.04.07
* @author jk   2014.05.19
*/
class COH_EXPORT BetweenFilter
    : public class_spec<BetweenFilter,
      extends<AndFilter> >
    {
    friend class factory<BetweenFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        BetweenFilter();

        /**
        * Construct a BetweenFilter for testing "Between" condition.
        *
        * @param vExtractor  the ValueExtractor to use by this filter
        * @param vFrom      the object to compare the "Greater or Equals"
        *                   boundary with
        * @param vTo        the object to compare the "Less or Equals" boundary
        *                   with
        */
        BetweenFilter(ValueExtractor::View vExtractor, Comparable::View vFrom,
                Comparable::View vTo);

        /**
         * Construct a BetweenFilter for testing "Between" condition.
         *
         * @param vExtractor          The ValueExtractor to be used by this filter
         * @param vLowerBound         The lower bound of the range
         * @param vUpperBound         The upper bound of the range
         * @param fIncludeLowerBound  A flag indicating whether values matching
         *                            the lower bound evaluate to true
         * @param fIncludeUpperBound  A flag indicating whether values matching
         *                            the upper bound evaluate to true
         */
        BetweenFilter(ValueExtractor::View vExtractor, Comparable::View vLowerBound,
                Comparable::View vUpperBound, bool fIncludeLowerBound,
                bool fIncludeUpperBound);

    private:
        /**
        * Blocked copy constructor.
        */
        BetweenFilter(const BetweenFilter&);

    // ----- EntryFilter interface ------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual bool evaluate(Object::View v) const;

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

    // ----- Object interface -----------------------------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual TypedHandle<const String> toString() const;

    // ----- helper methods -------------------------------------------------

    protected:

        virtual Filter::View applySortedIndex(SortedMap::View vIndexContent,
            Set::Handle hSetKeys) const;

        virtual bool evaluateExtracted(Object::View vExtracted) const;

    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the ValueExtractor used by this filter.
        *
        * @return the ValueExtractor::View used by this filter
        */
        virtual ValueExtractor::View getValueExtractor() const;

        /**
         * Obtain the lower bound of the range used by this filter.
         *
         * @return the lower bound Object::View used by this filter
         */
        virtual Comparable::View getLowerBound() const;

        /**
         * Obtain the upper bound of the range used by this filter.
         *
         * @return the upper bound Object::View used by this filter
         */
        virtual Comparable::View getUpperBound() const;

        /**
         * Obtain the flag indicating whether extracted values matching the
         * lower bound of the range used by this filter will evaluate to true.
         *
         * @return the flag indicating whether extracted values matching the
         * lower bound of the range used by this filter will evaluate to true.
         */
        virtual bool isLowerBoundInclusive() const;

        /**
         * Obtain the flag indicating whether extracted values matching the
         * upper bound of the range used by this filter will evaluate to true.
         *
         * @return the flag indicating whether extracted values matching the
         * upper bound of the range used by this filter will evaluate to true.
         */
        virtual bool isUpperBoundInclusive() const;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_BETWEEN_FILTER_HPP
