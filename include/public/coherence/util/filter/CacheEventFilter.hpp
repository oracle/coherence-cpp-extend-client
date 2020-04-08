/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CACHE_EVENT_FILTER_HPP
#define COH_CACHE_EVENT_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/util/filter/MapEventFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
 * An extension of the MapEventFilter which allows selection of client
 * driven (natural) events, cache internal (synthetic) events, or both.
 *
 * @author rhl 2013.04.10
 */
class COH_EXPORT CacheEventFilter
    : public class_spec<CacheEventFilter,
        extends<MapEventFilter> >
    {
    friend class factory<CacheEventFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        CacheEventFilter();

        /**
        * Construct a CacheEventFilter that evaluates MapEvent objects based on the
        * specified combination of event types.
        *
        * @param vFilter         the filter passed previously to a keySet() query method
        * @param nMaskSynthetic  any combination of e_synthetic and e_natural
        */
        CacheEventFilter(int32_t nMask, int32_t nMaskSynthetic);

        /**
        * Construct a CacheEventFilter that evaluates MapEvent objects that would
        * affect the results of a keySet filter issued by a previous call to
        * coherence::util::QueryMap::keySet(coherence::util::Filter::View).
        * It is possible to easily implement <i>continuous query</i> functionality.
        * <p>
        * Using this constructor is equivalent to:
        * <tt>
        * CacheEventFilter::create(e_keyset, vFilter, nMaskSynthetic);
        * </tt>
        *
        * @param vFilter         the filter passed previously to a keySet() query method
        * @param nMaskSynthetic  any combination of e_synthetic and e_natural
        */
        CacheEventFilter(Filter::View vFilter, int32_t nMaskSynthetic);

        /**
        * Construct a CacheEventFilter that evaluates MapEvent objects based on the
        * specified combination of event types.
        *
        * @param nMask           any combination of e_inserted, e_updated, e_deleted,
        *                        e_updated_entered, e_updated_within, and e_updated_left
        * @param vFilter         the filter passed previously to a keySet() query method
        * @param nMaskSynthetic  any combination of e_synthetic and e_natural
        */
        CacheEventFilter(int32_t nMask, Filter::View vFilter, int32_t nMaskSynthetic);

    private:
        /**
        * Blocked copy constructor.
        */
        CacheEventFilter(const CacheEventFilter&);


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


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


    // ----- Describable interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getDescription() const;


    // ----- constants ------------------------------------------------------

    public:
        /**
        * Bitmask to select synthetic events.
        */
        static const int32_t e_synthetic = 0x1;

        /**
        * Bitmask to select natural events.
        */
        static const int32_t e_natural   = 0x2;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Bitmask that selects whether to include synthetic, natural, or all events.
        */
        int32_t m_nMaskSynthetic;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_CACHE_EVENT_FILTER_HPP
