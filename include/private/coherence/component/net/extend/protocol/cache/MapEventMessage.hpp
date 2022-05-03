/*
 * Copyright (c) 2000, 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MAP_EVENT_MESSAGE_HPP
#define COH_MAP_EVENT_MESSAGE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "private/coherence/component/net/extend/AbstractPofMessage.hpp"



COH_OPEN_NAMESPACE6(coherence,component,net,extend,protocol,cache)

using coherence::component::net::extend::AbstractPofMessage;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;


/**
* Message that encapsulates the information in a MapEvent. This Message is
* sent by a MapListener registered on the remote NamedCache by the peer
* NamedCacheProxy.
*
* NOTE: Renamed from MapEvent due to a build issue where an .obj file name
* conflict occurs for classes with the same name even if they are in
* different namespaces.
*
* @author js  2008.07.07
*/
class COH_EXPORT MapEventMessage
    : public class_spec<MapEventMessage,
        extends<AbstractPofMessage> >
    {
    friend class factory<MapEventMessage>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new MapEventMessage instance.
        */
        MapEventMessage();


    // ----- Runnable interface ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void run();


    // ----- Message interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual int32_t getTypeId() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isExecuteInOrder() const;


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


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Set MapEventMessage identifier to one of entry_inserted,
        * entry_updated, entry_deleted.
        *
        * @param nId  the MapEventMessage id
        */
        virtual void setId(int32_t nId);

        /**
        * Get the MapEventMessage id.
        *
        * @return the MapEventMessage id which will be one of
        *         entry_inserted,entry_updated, entry_deleted.
        */
        virtual int32_t getId() const;

        /**
        * Set the filter id.
        *
        * @param lFilterId  the filter id that caused the MapEventMessage to
        *                   be raised.
        */
        virtual void setFilterId(int64_t lFilterId);

        /**
        * Get the filter id that caused the MapEventMessage to be raised.
        *
        * @return the filter id
        */
        virtual int64_t getFilterId() const;

        /**
        * Set the filter ids.
        *
        * @param valFilterIds  the filter ids that caused the MapEventMessage to
        *                      be raised.
        * @since Coherence 3.7.1.8
        */
        virtual void setFilterIds(Array<int64_t>::View valFilterIds);

        /**
        * Get the filter ids that caused the MapEventMessage to be raised.
        *
        * @since Coherence 3.7.1.8
        *
        * @return the filter ids
        */
        virtual Array<int64_t>::View getFilterIds() const;

        /**
        * Set the key associated with the MapEventMessage.
        *
        * @param vKey  the MapEventMessage key
        */
        virtual void setKey(Object::View vKey);

        /**
        * Get the key associated with the MapEventMessage.
        *
        * @return the MapEventMessage key
        */
        virtual Object::View getKey() const;

        /**
        * Set the new value (for insert and update events).
        *
        * @param vNewVal  the new value
        */
        virtual void setValueNew(Object::View vNewVal);

        /**
        * Get the new value (for insert and update events).
        *
        * @return the new value
        */
        virtual Object::View getValueNew() const;

        /**
        * Set the old value (for update and delete events).
        *
        * @param vOldVal  the old value
        */
        virtual void setValueOld(Object::View vOldVal);

        /**
        * Get the old value (for update and delete events).
        *
        * @return the old value
        */
        virtual Object::View getValueOld() const;

        /**
        * Set whether the event is synthetic.
        *
        * @param fSynthetic  true if the event is synthetic
        */
        virtual void setSynthetic(bool fSynthetic);

        /**
        * Get whether the event is synthetic.
        *
        * @return true if the event is synthetic, false otherwise
        */
        virtual bool isSynthetic() const;

        /**
        * Set the event Transformation state
        *
        * @param transformation state
        */
        virtual void setTransformState(int32_t nTransformState);

        /**
        * Get the event Transformation state
        *
        * @return transformation state
        */
        virtual int32_t getTransformState() const;

        /** Set whether the request is for cache truncate.
        *
        * @param fTruncate  true if MapEvent is caused by truncate operation
        */
        virtual void setTruncate(bool fTruncate);

        /**
        * Determine whether the MapEvent is caused by truncate operation.
        *
        * @return true if the MapEvent is caused by truncate operation
        */
        virtual bool isTruncate() const;

        /**
        * Set whether or not the event is a priming CacheEvent.
        *
        * @param fPriming  true if the event is synthetic
        */
        virtual void setPriming(bool fPriming);

        /**
        * Determine whether or not the event is a priming CacheEvent.
        *
        * @return true if the MapEvent is a priming CacheEvent
        */
        virtual bool isPriming() const;

        /**
        * Set whether or not the event was caused by a time-based eviction.
        *
        * @param fExpired  true if the event was caused by a time-based evistion
        * @since 14.1.1.0.10
        */
        virtual void setExpired(bool fExprired);

        /**
        * Determine whether or not the event was caused by a time-based eviction.
        *
        * @return true f the MapEvent was caused by a time-based eviction
        * @since 14.1.1.0.10
        */
        virtual bool isExpired() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * The type identifier of this Message class.
        */
        static const int32_t type_id = 13;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The MapEventMessage identifier; one of entry_inserted,
        * entry_updated, entry_deleted.
        */
        int32_t m_nId;

        /**
        * Filter Id. If positive, identifies the Filter that caused the
        * MapEventMessage to be raised.
        *
        * @since Coherence 3.7.1.8
        */
        int64_t m_lFilterId;

        /**
        * The key associated with the MapEventMessage.
        */
        FinalView<Object> f_vKey;

        /**
        * The new value (for insert and update events).
        */
        FinalView<Object> f_vNewValue;

        /**
        * The old value (for update and delete events).
        */
        FinalView<Object> f_vOldValue;

        /**
        * True if the MapEventMessage was caused by cache internal processing
        * such as eviction or loading.
        */
        bool m_fSynthetic;

        /**
         * Filter Ids.  If positive, identifies the Filters that caused the
         * MapEventMessage to be raised.
         */
        FinalView<Array<int64_t> > f_alFilterIds;

        /**
        * Transformation state of the event.
        */
        int32_t m_nTransformState;

        /**
        * True if the MapEvent is caused by truncate operation.
        */
        bool m_fTruncate;

        /**
        * True if the event is a priming event (NearCache).
        */
        bool m_fPriming;

        /**
        * Flag indicating an event resulting from expiration.
        * @since 14.1.1.0.10
        */
        bool m_fExpired;
    };

COH_CLOSE_NAMESPACE6

#endif //COH_MAP_EVENT_MESSAGE_HPP

