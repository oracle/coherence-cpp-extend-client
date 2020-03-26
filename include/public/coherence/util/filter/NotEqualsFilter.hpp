/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NOTEQUALS_FILTER_HPP
#define COH_NOTEQUALS_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which compares the result of a method invocation with a value for
* inequality.
*
* @author djl  2008.03.23
*/
class COH_EXPORT NotEqualsFilter
    : public class_spec<NotEqualsFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<NotEqualsFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        NotEqualsFilter();

        /**
        * Construct an NotEqualsFilter for testing inequality.
        *
        * @param vValueExtractor  the ValueExtractor to use by this filter
        * @param vValue           the object to compare the result with
        */
        NotEqualsFilter(ValueExtractor::View vValueExtractor,
                Object::View vValue);

    private:
        /**
        * Blocked copy constructor.
        */
        NotEqualsFilter(const NotEqualsFilter&);


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

#endif // COH_NOTEQUALS_FILTER_HPP
