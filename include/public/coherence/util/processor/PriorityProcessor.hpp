/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRIORITY_PROCESSOR_HPP
#define COH_PRIORITY_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/net/AbstractPriorityTask.hpp"
#include "coherence/util/InvocableMap.hpp"

COH_OPEN_NAMESPACE3(coherence,util,processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::AbstractPriorityTask;
using coherence::util::InvocableMap;


/**
* PriorityProcessor is used to explicitly control the scheduling priority and
* timeouts for execution of EntryProcessor-based methods.
*
* @author tb  2008.05.23
*/
class COH_EXPORT PriorityProcessor
    : public class_spec<PriorityProcessor,
        extends<AbstractPriorityTask>,
        implements<InvocableMap::EntryProcessor, PortableObject> >
    {
    friend class factory<PriorityProcessor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a PriorityProcessor.
        *
        * @return a new PriorityProcessor
        */
        PriorityProcessor();

        /**
        * Construct a PriorityProcessor.
        *
        * @param processor  the processor wrapped by this PriorityProcessor
        */
        PriorityProcessor(InvocableMap::EntryProcessor::View vProcessor);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::View processAll(Set::View vSetEntries) const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the underlying processor.
        *
        * @return the processor wrapped by this PriorityProcessor
        */
        virtual InvocableMap::EntryProcessor::View getProcessor() const;


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
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying entry processor.
        */
        FinalView<InvocableMap::EntryProcessor> f_vProcessor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PRIORITY_PROCESSOR_HPP
