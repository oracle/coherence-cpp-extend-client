/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_LISTENER_FILTER_REQUEST_HPP
#define COH_LISTENER_FILTER_REQUEST_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/util/MapTrigger.hpp"

#include "private/coherence/component/net/extend/AbstractPofResponse.hpp"
#include "private/coherence/component/net/extend/protocol/cache/AbstractFilterRequest.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofResponse;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::util::MapTrigger;


/**
* A request which is sent to register or de-register interest in events that
* pass a specified filter on a remote NamedCache.
*
* @author js  2008.07.08
*/
class COH_EXPORT ListenerFilterRequest
    : public class_spec<ListenerFilterRequest,
        extends<AbstractFilterRequest> >
    {
    friend class factory<ListenerFilterRequest>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ListenerFilterRequest instance.
        */
        ListenerFilterRequest();


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
        * Set the filter id.
        *
        * @param lFilterId  a unique identifier for the filter associated
        *                   with this ListenerFilterRequest
        */
        virtual void setFilterId(int64_t lFilterId);

        /**
        * Get the filter id.
        *
        * @return the unique identifier for the filter associated with this
        *         ListenerFilterRequest
        */
        virtual int64_t getFilterId() const;

        /**
        * Set whether a listener is being being added.
        *
        * @param fAdd  true to add a MapListener, false to remove a
        *              MapListener
        */
        virtual void setAdd(bool fAdd);

        /**
        * Determine whether a listener is being added.
        *
        *
        * @return true if adding a MapListener, false if removing a
        *         MapListener
        */
        virtual bool isAdd() const;

        /**
        * Set whether the MapListener is "lite".
        *
        * @param fLite  true if the MapListener is "lite", false if it is a
        *               standard MapListener
        */
        virtual void setLite(bool fLite);

        /**
        * Determine whether the MapListener is "lite".
        *
        * @return true if the MapListener is "lite", false if it is a
        *         standard MapListener
        */
        virtual bool isLite() const;

        /**
        * Set the associated MapTrigger.
        *
        * @param hTrigger  the trigger to associate with this request
        */
        virtual void setTrigger(MapTrigger::Handle hTrigger);

        /**
        * Get the MapTrigger associated with this request.
        *
        * @return the trigger associated with this request; may be NULL
        */
        virtual MapTrigger::Handle getTrigger() const;

        /**
        * Set whether the MapListener is "PrimingListener".
        *
        * @param fPriming  true if the MapListener is "PrimingListener", false if it is a
        *                  standard MapListener
        */
        virtual void setPriming(bool fPriming);

        /**
        * Determine whether the MapListener is "lite".
        *
        * @return true if the MapListener is a "PrimingListener", false if it is a
        *         standard MapListener
        */
        virtual bool isPriming() const;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 12;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A unique identifier for the filter associated with this
        * ListenerFilterRequest.
        */
        int64_t m_lFilterId;

        /**
        * Flag to indicate whether a listener is being added.
        */
        bool m_fAdd;

        /**
        * Flag to indicate whether the listener is "lite".
        */
        bool m_fLite;

        /**
        * An optional MapTrigger object associated with this request.
        */
        mutable FinalHandle<MapTrigger> f_hTrigger;
        
        /**
        * Support for the NearCache priming listener. The value of true indicates that the listener
        * registration should force a synthetic event containing the current value to the requesting client.
        * This property was added to Coherence 12.2.1 (protocol version 6) for COH-4615 implementation.
        */
        mutable bool m_fPriming;
    };

COH_CLOSE_NAMESPACE6

#endif //COH_LISTENER_FILTER_REQUEST_HPP

