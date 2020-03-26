/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONTAINS_FILTER_HPP
#define COH_CONTAINS_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/filter/ComparisonFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which tests a Collection or ObjectArray value returned from
* a method invocation for containment of a given value.
*
* More formally, if the specified extractor returns a Collection,
* evaluate(o)is functionally equivalent to the following code:
* <pre>
* return ((Collection) extract(o))->contains(getValue());
* </pre>
* If the specified method returns an ObjectArray, evaluate(o) is functionally
* equivalent to the following code:
* <pre>
* return Collections->asList((ObjectArray::View) extract(o))->contains(
*           getValue());
* </pre>
*
* @author djl  2008.04.07
*/
class COH_EXPORT ContainsFilter
    : public class_spec<ContainsFilter,
        extends<ComparisonFilter>,
        implements<IndexAwareFilter> >
    {
    friend class factory<ContainsFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ContainsFilter();

        /**
        * Construct an ContainsFilter for testing containment of the given
        * object.
        *
        * @param vExtractor  the ValueExtractor used by this filter
        * @param vValue      the object that a Collection or ObjectArray is
        *                    tested to contain
        */
        ContainsFilter(ValueExtractor::View vExtractor, Object::View vValue);

    private:
        /**
        * Blocked copy constructor.
        */
        ContainsFilter(const ContainsFilter&);


    // ----- ExtractorFilter interface --------------------------------------

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

#endif // COH_CONTAINS_FILTER_HPP
