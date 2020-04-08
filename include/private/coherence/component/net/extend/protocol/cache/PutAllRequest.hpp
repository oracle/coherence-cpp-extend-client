/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PUT_ALL_REQUEST_HPP
#define COH_PUT_ALL_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/NamedCacheRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::Map;


/**
* Map::putAll(Map::View vMapEntries) Request message.
*
* @author jh  2008.02.18
*/
class COH_EXPORT PutAllRequest
    : public class_spec<PutAllRequest,
        extends<NamedCacheRequest> >
    {
    friend class factory<PutAllRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new PutAllRequest instance.
        */
        PutAllRequest();


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;


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


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        */
        virtual void onRun(AbstractPofResponse::Handle hResponse);


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the Map of entries.
        *
        * @return the Map of entries
        */
        virtual Map::View getMap() const;

        /**
        * Configure the Map of entries.
        *
        * @param vMap  the Map of entries
        */
        virtual void setMap(Map::View vMap);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 7;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Map of entries to be updated when this message is processed.
        */
        mutable FinalView<Map> f_vMap;
    };

COH_CLOSE_NAMESPACE6

#endif // COH_PUT_ALL_REQUEST_HPP
