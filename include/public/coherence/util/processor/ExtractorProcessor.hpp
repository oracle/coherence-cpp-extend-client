/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXTRACTOR_PROCESSOR_HPP
#define COH_EXTRACTOR_PROCESSOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/InvocableMap.hpp"
#include "coherence/util/ValueExtractor.hpp"
#include "coherence/util/processor/AbstractProcessor.hpp"

COH_OPEN_NAMESPACE3(coherence,util, processor)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::util::InvocableMap;
using coherence::util::ValueExtractor;


/**
* ExtractorProcessor is an EntryProcessor implementations that extracts a
* value from an object cached in an InvocableMap.
*
* @author tb  2008.04.28
*/
class COH_EXPORT ExtractorProcessor
    : public class_spec<ExtractorProcessor,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<ExtractorProcessor>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ExtractorProcessor.
        */
        ExtractorProcessor();

        /**
        * Construct an ExtractorProcessor based on the specified
        * ValueExtractor.
        *
        * @param vExtractor  a ValueExtractor object; passing null is
        *                   equivalent to using the IdentityExtractor
        */
        ExtractorProcessor(ValueExtractor::View vExtractor);


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(InvocableMap::Entry::Handle hEntry) const;


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
        * The underlying value extractor.
        */
        FinalView<ValueExtractor> f_vExtractor;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EXTRACTOR_PROCESSOR_HPP
