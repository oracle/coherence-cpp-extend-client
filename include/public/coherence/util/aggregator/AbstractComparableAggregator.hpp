/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ABSTRACT_COMPARABLE_AGGREGATOR_HPP
#define COH_ABSTRACT_COMPARABLE_AGGREGATOR_HPP

#include "coherence/lang.ns"

#include "coherence/util/aggregator/AbstractAggregator.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE3(coherence,util,aggregator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

using coherence::util::Comparator;
using coherence::util::ValueExtractor;


/**
* Abstract aggregator that processes values extracted from a set of entries
* in a Map, with knowledge of how to compare those values. There are two ways
* to use the AbstractComparableAggregator:
* <ul>
* <li>All the extracted objects must implement <tt>Comparable</tt>, or</li>
* <li>The AbstractComparableAggregator has to be provided with a
* <tt>Comparator object</tt>.</li>
* </ul>
* If the set of entries passed to <tt>aggregate</tt> is empty, a
* <tt>NULL</tt> result is returned.
*
* @author djl/pp  2008.05.09
*/
class COH_EXPORT AbstractComparableAggregator
    : public abstract_spec<AbstractComparableAggregator,
        extends<AbstractAggregator> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        AbstractComparableAggregator();

        /**
        * Construct an AbstractComparableAggregator object.
        *
        * @param vExtractor   the extractor that provides an object to be 
        *                     compared
        * @param vComparator  the comparator used to compare the extracted
        *                     object, if NULL then the extracted object must
        *                     implement Comparable
        */
        AbstractComparableAggregator(ValueExtractor::View vExtractor,
                Comparator::View vComparator = NULL);

        /**
        * Construct an AbstractComparableAggregator that will aggregate 
        * values extracted from a set of InvocableMap::Entry objects.
        *
        * @param vsMethod     the name of the method that could be 
        *                     invoked via reflection and that returns values 
        *                     to aggregate; this parameter can also be a 
        *                     dot-delimited sequence of method names which 
        *                     would result in an aggregator based on the
        *                     ChainedExtractor that is based on an array of
        *                     corresponding ReflectionExtractor objects
        * @param vComparator  the comparator used to compare the extracted
        *                     object, if NULL then the extracted object must
        *                     implement Comparable
        *
        * @since Coherence 12.1.2
        */
        AbstractComparableAggregator(String::View vsMethod,
                Comparator::View vComparator = NULL);

    private:
        /**
        * Blocked copy constructor.
        */
        AbstractComparableAggregator(const AbstractComparableAggregator&);


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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The count of processed entries.
        */
        int32_t m_count;

        /**
        * The running result value.
        */
        MemberView<Object> m_vResult;

        /**
        * The comparator to use for comparing extracted values.
        */
        FinalView<Comparator> f_vComparator;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ABSTRACT_COMPARABLE_AGGREGATOR_HPP
