/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EQUALS_FILTER_HPP
#define COH_EQUALS_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which compares the result of a method invocation with a value for
* equality.
*
* @author djl  2008.03.23
*/
class COH_EXPORT EqualsFilter
    : public class_spec<EqualsFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<EqualsFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        EqualsFilter();

        /**
        * Construct an EqualsFilter for testing equality.
        *
        * @param vValueExtractor  the ValueExtractor to use by this filter
        * @param vValue           the object to compare the result with
        */
        EqualsFilter(ValueExtractor::View vValueExtractor,
                Object::View vValue);

    private:
        /**
        * Blocked copy constructor.
        */
        EqualsFilter(const EqualsFilter&);

		
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

#endif // COH_EQUALS_FILTER_HPP
