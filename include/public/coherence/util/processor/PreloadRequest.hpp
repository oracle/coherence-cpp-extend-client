/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PRELOAD_REQUEST_HPP
#define COH_PRELOAD_REQUEST_HPP

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
* PreloadRequest is a simple EntryProcessor that performs a
* InvocableMap::Entry#getValue() call. No results are reported back to the
* caller.
*
* The PreloadRequest process provides a means to "pre-load" an entry or a
* collection of entries into the cache using the cache's loader without
* incurring the cost of sending the value(s) over the network. If the
* corresponding entry (or entries) already exists in the cache, or if the
* cache does not have a loader, then invoking this EntryProcessor has no
* effect.
*
* @author tb  2008.04.28
*/
class COH_EXPORT PreloadRequest
    : public class_spec<PreloadRequest,
        extends<AbstractProcessor>,
        implements<PortableObject> >
    {
    friend class factory<PreloadRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a PreloadRequest.
        */
        PreloadRequest();


    // ----- InvocableMap::EntryProcessor interface -------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder process(
                InvocableMap::Entry::Handle hEntry) const;

        /**
        * {@inheritDoc
        */
        virtual Map::View processAll(Set::View vsetEntries) const;


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

    };

COH_CLOSE_NAMESPACE3

#endif // COH_PRELOAD_REQUEST_HPP
