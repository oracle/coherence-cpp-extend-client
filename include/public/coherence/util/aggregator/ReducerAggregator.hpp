/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_REDUCER_AGGREGATOR_HPP
#define COH_REDUCER_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractAggregator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)


/**
* The ReducerAggregator is used to implement functionality similar to
* <tt>CacheMap::getAll(Collection)</tt> API.  Instead of returning the complete
* set of values, it will return a portion of value attributes based on the
* provided <tt>ValueExtractor</tt>.
*
* This aggregator could be used in combination with
* <tt>coherence.util.extractor.MultiExtractor</tt> allowing one to collect
* tuples that are a subset of the attributes of each object stored in the cache.
*
* @author par 04.25.13
*/
class COH_EXPORT ReducerAggregator
    : public class_spec<ReducerAggregator,
        extends<AbstractAggregator> >
    {
    friend class factory<ReducerAggregator>;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        ReducerAggregator();

        /**
        * Construct a ReducerAggregator based on the specified extractor.
        *
        * @param vExtractor  the extractor that is used to extract the portion
        *                    of the cached value
        */
        ReducerAggregator(ValueExtractor::View vExtractor);

        /**
         * Construct a ReducerAggregator based on the specified method name.
         *
         * @param vsMethod  the name of the method that is used to extract the
         *                  portion of the cached value
         */
        ReducerAggregator(String::View vsMethod);

    private:
        /**
        * Blocked copy constructor.
        */
        ReducerAggregator(const ReducerAggregator&);


    // ----- AbstractAggregator methods -------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void init(bool fFinal);

        /**
        * {@inheritDoc}
        */
        virtual void process(Object::View vO, bool fFinal);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder finalizeResult(bool fFinal);


    // ----- internal helpers -----------------------------------------------

    protected:
        /**
        * Return a map that can be used to store reduced values, creating it if
        * one has not already been created.
        *
        * @return a set that can be used to store distinct values
        */
        virtual Map::Handle ensureMap();


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The resulting map of reduced values.
        */
        MemberHandle<Map> m_hMap;
    };

COH_CLOSE_NAMESPACE3

#endif //COH_REDUCER_AGGREGATOR_HPP
