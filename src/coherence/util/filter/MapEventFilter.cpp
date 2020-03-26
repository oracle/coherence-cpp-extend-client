/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/filter/MapEventFilter.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/MapEvent.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

COH_REGISTER_PORTABLE_CLASS(79, MapEventFilter);

COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_inserted)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_updated)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_deleted)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_updated_entered)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_updated_left)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_updated_within)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_all)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEventFilter::e_keyset)


// ----- constructors -------------------------------------------------------

MapEventFilter::MapEventFilter()
    : m_nMask(0), f_vFilter(self())
    {
    }

MapEventFilter::MapEventFilter(Filter::View vFilter)
    : m_nMask(e_keyset), f_vFilter(self(), vFilter)
    {
    }

MapEventFilter::MapEventFilter(int32_t nMask, Filter::View vFilter)
    : m_nMask(nMask), f_vFilter(self(), vFilter)
    {
    }


// ----- Filter interface ---------------------------------------------------

bool MapEventFilter::evaluate(Object::View v) const
    {
    MapEvent::View vEvent = cast<MapEvent::View>(v);

    // check if the event is of a type ("id") that the client is
    // interested in evaluating
    int32_t nId   = vEvent->getId();
    int32_t nMask = getEventMask();
    try
        {
        if ((getMask(nId) & nMask) == 0)
            {
            return false;
            }
        }
    catch (IndexOutOfBoundsException::View)
        {
        return false;
        }

    // check for a client-specified event filter
    Filter::View vFilter = getFilter();
    if (vFilter == NULL)
        {
        return true;
        }

    // evaluate the filter
    switch (nId)
        {
        case MapEvent::entry_inserted:
            return vFilter->evaluate(vEvent->getNewValue());

        case MapEvent::entry_updated:
            {
            // note that the old value evaluation is deferred, because
            // the event itself may be deferring loading the old value,
            // e.g. if the event is coming from a disk-backed cache
            bool fNew = vFilter->evaluate(vEvent->getNewValue());

            switch (nMask & (e_updated_entered | e_updated_left |
                             e_updated | e_updated_within))
                {
                case e_updated_entered:
                    return fNew && !vFilter->evaluate(vEvent->getOldValue());

                case e_updated_left:
                    return !fNew && vFilter->evaluate(vEvent->getOldValue());

                case e_updated_entered | e_updated_left:
                    return fNew != vFilter->evaluate(vEvent->getOldValue());

                case e_updated_within:
                    return fNew && vFilter->evaluate(vEvent->getOldValue());

                case e_updated_within | e_updated_entered:
                    return fNew;

                case e_updated_within | e_updated_left:
                    return vFilter->evaluate(vEvent->getOldValue());

                default:
                    // all other combinations evaulate to the same as
                    // e_updated
                    return fNew || vFilter->evaluate(vEvent->getOldValue());
                }
            }

        case MapEvent::entry_deleted:
            return vFilter->evaluate(vEvent->getOldValue());

        default:
            return false;
        }
    }


// ----- PortableObject interface -------------------------------------------

void MapEventFilter::readExternal(PofReader::Handle hIn)
    {
    m_nMask = hIn->readInt32(0);
    initialize(f_vFilter, cast<Filter::View>(hIn->readObject(1)));
   }

void MapEventFilter:: writeExternal(PofWriter::Handle hOut) const
    {
    hOut->writeInt32(0,m_nMask);
    hOut->writeObject(1, f_vFilter);
    }


// ----- Object interface ---------------------------------------------------

bool MapEventFilter::equals(Object::View v) const
    {
    if (NULL != v && Class::getClassName(this)->equals(Class::getClassName(v)))
        {
        MapEventFilter::View vThat = cast<MapEventFilter::View>(v);
        return m_nMask == vThat->m_nMask
                  && Object::equals(f_vFilter, vThat->f_vFilter);
        }
    return false;
    }

size32_t MapEventFilter::hashCode() const
    {
    int32_t nHash       = (size32_t) m_nMask;
    Filter::View vFilter = f_vFilter;
    if (vFilter != NULL)
        {
        nHash += vFilter->hashCode();
        }
    return nHash;
    }

TypedHandle<const String> MapEventFilter::toString() const
    {
    String::View vsFilter = Class::getClassName(this);

    return COH_TO_STRING(vsFilter->substring(vsFilter->lastIndexOf('.') + 1) << 
        '(' << getDescription() << ')');
    }

String::View MapEventFilter::getDescription() const
    {
    int32_t nMask = getEventMask();

    String::View vs = "mask=";
    if (nMask == e_all)
        {
        vs = COH_TO_STRING(vs << "ALL");
        }
    else if (nMask == e_keyset)
        {
        vs = COH_TO_STRING(vs << "KEYSET");
        }
    else
        {
        if ((nMask & e_inserted) != 0)
            {
            vs = COH_TO_STRING(vs << "INSERTED|");
            }
        if ((nMask & e_updated) != 0)
            {
            vs = COH_TO_STRING(vs << "UPDATED|");
            }
        if ((nMask & e_deleted) != 0)
            {
            vs = COH_TO_STRING(vs << "DELETED|");
            }
        if ((nMask & e_updated_entered) != 0)
            {
            vs = COH_TO_STRING(vs << "UPDATED_ENTERED|");
            }
        if ((nMask & e_updated_left) != 0)
            {
            vs = COH_TO_STRING(vs << "UPDATED_LEFT|");
            }
        if ((nMask & e_updated_within) != 0)
            {
            vs = COH_TO_STRING(vs << "UPDATED_WITHIN|");
            }
        }

    Filter::View vFilter = getFilter();
    if (vFilter != NULL)
        {
        vs = COH_TO_STRING(vs << ", filter=" << vFilter);
        }

    return vs;
    }


// ----- data members accessor-----------------------------------------------

int32_t MapEventFilter::getEventMask() const
    {
    return m_nMask;
    }

Filter::View MapEventFilter::getFilter() const
    {
    return f_vFilter;
    }

// ---- constants -----------------------------------------------------------

int32_t MapEventFilter::getMask(size32_t i)
    {
    switch (i)
        {
        case 0:
            return 0;
        case 1:
            return  e_inserted;
        case 2:
            return  e_updated | e_updated_within |
                e_updated_entered | e_updated_left;
        case 3:
            return  e_deleted;
        default:
            COH_THROW_STREAM(IndexOutOfBoundsException,
                    "getMask, Index out of bounds: " << i);
        }
    }

COH_CLOSE_NAMESPACE3
