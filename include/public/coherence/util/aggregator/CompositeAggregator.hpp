/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPOSITE_AGGREGATOR_HPP
#define COH_COMPOSITE_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* CompositeAggregator provides an ability to execute a collection of
* aggregators against the same subset of the entries in an InvocableMap,
* resulting in a list of corresponding aggregation results. The size of the
* returned list will always be equal to the length of the aggregators' array.
*
* @author djl  2008.05.16
*/
class COH_EXPORT CompositeAggregator
    : public class_spec<CompositeAggregator,
        extends<Object>,
        implements<PortableObject, InvocableMap::EntryAggregator> >
    {
    friend class factory<CompositeAggregator>;

    // ----- factory methods ------------------------------------------------

    public:
         /**
         * Default constructor (necessary for the PortableObject interface).
         */
         static CompositeAggregator::Handle create();

         /**
         * Create an instance of CompositeAggregator based on a specified
         * array of coherence::util::InvocableMap::EntryAggregator.
         *
         * @param vaAggregator  an array of EntryAggregator objects; must
         *                      contain not less than two aggregators
         */
         static CompositeAggregator::Handle create(
                 ObjectArray::View vaAggregator);


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        CompositeAggregator();

        /**
        * @internal
        */
        CompositeAggregator(ObjectArray::View vaAggregator);

    private:
        /**
        * Blocked copy constructor.
        */
        CompositeAggregator(const CompositeAggregator&);


    // ----- InvocableMap::EntryAggregator interface ------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder aggregate(Set::View vSetEntries);


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
        virtual ObjectArray::View getAggregators() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying EntryAggregator array.
        */
        FinalView<ObjectArray> f_vaAggregator;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_COMPOSITE_AGGREGATOR_HPP
