/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_AGGREGATOR_HPP
#define COH_ABSTRACT_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Abstract base class implementation of
* coherence::util::InvocableMap::EntryAggregator that supports
* parallel aggregation.
*
* For aggregators which only run within the Coherence cluster
* (most common case), the C++ init, process, finalizeResult, aggregate,
* and aggregateResults methods can be left unimplemented.
*
* @author djl  2008.05.09
*/
class COH_EXPORT AbstractAggregator
    : public abstract_spec<AbstractAggregator,
        extends<Object>,
        implements<PortableObject, InvocableMap::ParallelAwareAggregator> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractAggregator();

        /**
        * @internal
        */
        AbstractAggregator(ValueExtractor::View vExtractor);

        /**
        * Construct an AbstractAggregator that will aggregate values 
        * extracted from a set of InvocableMap::Entry objects.
        *
        * @param vsMethod  the name of the method that could be invoked
        *                  via reflection and that returns values to 
        *                  aggregate; this parameter can also be a 
        *                  dot-delimited sequence of method names which 
        *                  would result in an aggregator based
        *                  on the ChainedExtractor that is based on an
        *                  array of corresponding ReflectionExtractor
        *                  objects
        *
        * @since Coherence 12.1.2
        */
        AbstractAggregator(String::View vsMethod);

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractAggregator(const AbstractAggregator&);


    // ----- AbstractAggregator Interface  ----------------------------------

    protected:
        /**
        * Initialize the aggregation result.
        *
        * This implementation throws an UnsupportedOperationException.
        *
        * @param fFinal  true is passed if the aggregation process that is
        *                being initialized must produce a final aggregation
        *                result; this will only be false if a parallel
        *                approach is being used and the initial (partial)
        *                aggregation process is being initialized
        */
        virtual void init(bool fFinal);

        /**
        * Incorporate one aggregatable value into the result.
        *
        * If the <tt>fFinal</tt> parameter is true, the given object is a
        * partial result (returned by an individual parallel aggregator) that
        * should be incorporated into the final result; otherwise, the object
        * is a value extracted from an
        * coherence::util::InvocableMap::Entry.
        *
        * This implementation throws an UnsupportedOperationException.
        *
        * @param vO      the value to incorporate into the aggregated result
        * @param fFinal  true to indicate that the given object is a partial
        *                result returned by a parallel aggregator
        */
        virtual void process(Object::View vO, bool fFinal);

        /**
        * Obtain the result of the aggregation.
        *
        * If the <tt>fFinal</tt> parameter is true, the returned object must
        * be the final result of the aggregation; otherwise, the returned
        * object will be treated as a partial result that should be
        * incorporated into the final result.
        *
        * This implementation throws an UnsupportedOperationException.
        *
        * @param  fFinal  true to indicate that the final result of the
        *                 aggregation process should be returned; this will
        *                 only be false if a parallel approach is being used
        *
        * @return the result of the aggregation process
        */
        virtual Object::Holder finalizeResult(bool fFinal);


    // ----- InvocableMap::EntryAggregator interface ------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Set::View vSetEntries);


    // ----- InvocableMap::ParallelAwareAggregator interface ----------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual InvocableMap::EntryAggregator::Handle
                getParallelAggregator();

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregateResults(
                Collection::View vCollResults);


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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Determine the ValueExtractor whose values this aggregator is
        * aggregating.
        *
        * @return the ValueExtractor used by this aggregator
        */
        virtual ValueExtractor::View getValueExtractor() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Set to true if this aggregator realizes that it is going to be used
        * in parallel.
        */
        bool m_fParallel;

        /**
        * The ValueExtractor that obtains the value to aggregate from the
        * value that is stored in the Map.
        */
        FinalView<ValueExtractor> f_vExtractor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_AGGREGATOR_HPP
