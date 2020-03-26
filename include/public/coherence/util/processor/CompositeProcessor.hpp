/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPOSITE_PROCESSOR_HPP
#define COH_COMPOSITE_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;


/**
* The CompositeProcessor represents a collection of entry processors that are
* invoked sequentially against the same Entry.
*
* @author tb  2008.04.28
*/
class COH_EXPORT CompositeProcessor
    : public class_spec<CompositeProcessor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<CompositeProcessor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a CompositeProcessor.
        */
        CompositeProcessor();

        /**
        * Construct a CompositeProcessor for the specified array of
        * individual entry processors.
        *
        * @param vaProcessor  the entry processor array
        */
        CompositeProcessor(ObjectArray::View vaProcessor);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(
                InvocableMap::Entry::Handle hEntry) const;


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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying entry processor array.
        */
        FinalView<ObjectArray> f_vaProcessor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_COMPOSITE_PROCESSOR_HPP
