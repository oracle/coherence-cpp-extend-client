/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SLOW_PROCESSOR_HPP
#define COH_SLOW_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/InvocableMap.hpp"

#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using namespace coherence::lang;

using coherence::util::processor::AbstractProcessor;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;

/**
 * Test entry processor that takes 10 seconds
 * to execute on the server.
 */
class SlowProcessor
        : public class_spec<SlowProcessor,
                extends<AbstractProcessor>,
                implements<PortableObject> >
    {
    protected:
        friend class factory<SlowProcessor>;

    // ----- constructors ---------------------------------------------------

    protected:
        SlowProcessor();

        SlowProcessor(int32_t nProcessTimeMillis, String::View vsReturnValue);

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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The time that the processor takes to execute on the server.
        */
        int32_t m_nProcessTimeMillis;

        /**
        * The value to return from the process method
        */
        FinalView<String> f_vsReturnValue;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SLOW_PROCESSOR_HPP
