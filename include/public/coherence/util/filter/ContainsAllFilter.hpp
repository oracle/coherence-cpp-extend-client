/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONTAINS_ALL_FILTER_HPP
#define COH_CONTAINS_ALL_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Set.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which tests a Collection or ObjectArray value returned from
* a method invocation for containment of all values in a Set.
*
* More formally, if the specified extractor returns a Collection,
* #evaluate is functionally equivalent to the following
* code:
* <pre>
* return ((Collection) extract(o))->containsAll((Collection) getValue());
* </pre>
* If the specified method returns an ObjectArray, #evaluate is functionally
* equivalent to the following code:
* <pre>
* return Collections->asList((ObjectArray::View) extract(o))->containsAll(
*        (Collection) getValue());
* </pre>
* @author djl  2008.04.07
*/
class COH_EXPORT ContainsAllFilter
    : public class_spec<ContainsAllFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<ContainsAllFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ContainsAllFilter();

        /**
        * Construct an ContainsAllFilter for testing containment of the given
        * Set of values.
        *
        * @param vExtractor  the ValueExtractor used by this filter
        * @param vSetValues  the Set of values that a Collection or ObjectArray
        *                    is tested to contain
        */
        ContainsAllFilter(ValueExtractor::View vExtractor,
                Set::View vSetValues);

    private:
        /**
        * Blocked copy constructor.
        */
        ContainsAllFilter(const ContainsAllFilter&);


    // ----- ContainsAllFilter interface ------------------------------------

    protected:
        /**
        * Evaluate the specified extracted value.
        *
        * @param vExtracted an extracted value to evaluate
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

#endif // COH_CONTAINS_ALL_FILTER_HPP
