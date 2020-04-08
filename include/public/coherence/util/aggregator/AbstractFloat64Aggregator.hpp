/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_FLOAT64_AGGREGATOR_HPP
#define COH_ABSTRACT_FLOAT64_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractAggregator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* Abstract aggregator that processes numeric values extracted from a set of
* entries in a Map. All the extracted Number objects will be treated as
* <tt>float64_t</tt> values and the result of the aggregator is a Float64.
* If the set of entries is empty, a <tt>NULL</tt> result is returned.
*
* @author djl  2008.05.09
*/
class COH_EXPORT AbstractFloat64Aggregator
    : public abstract_spec<AbstractFloat64Aggregator,
        extends<AbstractAggregator> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractFloat64Aggregator();

        /**
        * @internal
        */
        AbstractFloat64Aggregator(ValueExtractor::View vExtractor);

        /**
        * Construct an AbstractFloat64Aggregator that will aggregate values 
        * extracted from a set of InvocableMap::Entry objects.
        *
        * @param vsMethod  the name of the method that could be invoked via
        *                  reflection and that returns values to aggregate;
        *                  this parameter can also be a dot-delimited 
        *                  sequence of method names which would result in
        *                  an aggregator based on the ChainedExtractor that
        *                  is based on an array of corresponding 
        *                  ReflectionExtractor objects
        *
        * @since Coherence 12.1.2
        */
        AbstractFloat64Aggregator(String::View vsMethod);

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractFloat64Aggregator(const AbstractFloat64Aggregator&);

		
    // ----- AbstractAggregator Interface  ----------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void init(bool fFinal);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder finalizeResult(bool fFinal);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The count of processed entries.
        */
        int32_t m_count;

        /**
        * The running result value.
        */
        float64_t m_dflResult;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_FLOAT64_AGGREGATOR_HPP
