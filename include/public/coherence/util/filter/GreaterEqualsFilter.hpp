/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GREATER_EQUALS_FILTER_HPP
#define COH_GREATER_EQUALS_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which compares the result of a method invocation with a value for
* "Greater or Equals" condition. In a case when either result of a method
* invocation or a value to compare are equal to NULL, the <tt>evaluate</tt>
* test yields <tt>false</tt>. This approach is equivalent to the way
* the NULL values are handled by SQL.
*
* @author djl  2008.03.24
*/
class COH_EXPORT GreaterEqualsFilter
    : public class_spec<GreaterEqualsFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<GreaterEqualsFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        GreaterEqualsFilter();

        /**
        * Construct a GreaterEqualsFilter for testing "Greater or Equal"
        * condition.
        *
        * @param vValueExtractor  the ValueExtractor to use by this filter
        * @param vComparable      the object to compare the result with
        */
        GreaterEqualsFilter(ValueExtractor::View vValueExtractor,
                Comparable::View vComparable);

    private:
        /**
        * Blocked copy constructor.
        */
        GreaterEqualsFilter(const GreaterEqualsFilter&);


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
    };

COH_CLOSE_NAMESPACE3

#endif // COH_LESS_GREATER_EQUALS_HPP
