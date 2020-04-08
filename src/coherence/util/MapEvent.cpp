/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/util/MapEvent.hpp"

#include "coherence/util/MapListener.hpp"
#include "coherence/util/ObservableMap.hpp"

#include "private/coherence/util/logging/Logger.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


// ----- constructors -------------------------------------------------------

MapEvent::MapEvent(TypedHandle<ObservableMap> hMap, int32_t nId,
        Object::View vKey, Object::View vValueOld, Object::View vValueNew)
        : super(hMap),
          m_nId(nId),
          m_voKey(self(), vKey, /*fMutable*/ true),
          m_voValueOld(self(), vValueOld, /*fMutable*/ true),
          m_voValueNew(self(), vValueNew, /*fMutable*/ true)
    {
    }


// ----- MapEvent interface -------------------------------------------------

TypedHandle<ObservableMap> MapEvent::getMap() const
    {
    return cast<ObservableMap::Handle>(getSource());
    }

int32_t MapEvent::getId() const
    {
    return m_nId;
    }

Object::View MapEvent::getKey() const
    {
    return m_voKey;
    }

Object::View MapEvent::getOldValue() const
    {
    return m_voValueOld;
    }

Object::View MapEvent::getNewValue() const
    {
    return m_voValueNew;
    }


// ----- Object interface ---------------------------------------------------

TypedHandle<const String> MapEvent::toString() const
    {
    String::View vsEvt    = Class::getClassName(this);
    String::View vsSource = Class::getClassName(getSource());

    return COH_TO_STRING(vsEvt->substring(vsEvt->lastIndexOf('.') + 1) << '{' <<
            vsSource->substring(vsSource->lastIndexOf('.') + 1) <<
            getDescription() << '}');
    }


// ----- helper methods -----------------------------------------------------

void MapEvent::dispatch(Listeners::View vListeners) const
    {
    dispatch(vListeners, true);
    }

void MapEvent::dispatch(Listeners::View vListeners, bool fStrict) const
    {
    if (vListeners != NULL)
        {
        ObjectArray::View vTargets = vListeners->listeners();

        for (size32_t i = vTargets->length; i-- != 0;)
            {
            MapListener::Handle hTarget =
                    cast<MapListener::Handle>(vTargets[i]);
            try
                {
                if (shouldDispatch(hTarget))
                    {
                    dispatch(hTarget);
                    }
                }
            catch (Exception::View e)
                {
                if (fStrict)
                    {
                    COH_THROW (e);
                    }
                else
                    {
                    COH_LOGEXMSG (e, "during event dispatching", 2);
                    }
                }
            }
        }
    }

void MapEvent::dispatch(TypedHandle<MapListener> hListener) const
    {
    if (shouldDispatch(hListener))
        {
        switch (getId())
            {
            case entry_inserted:
                hListener->entryInserted(this);
                break;

            case entry_updated:
                hListener->entryUpdated(this);
                break;

            case entry_deleted:
                hListener->entryDeleted(this);
                break;
             }
         }
    }

String::View MapEvent::getDescription(int32_t nId)
    {
    switch (nId)
        {
        case entry_inserted:
            return "inserted";

        case entry_updated:
            return "updated";

        case entry_deleted:
            return "deleted";

        default:
            return COH_TO_STRING("<unknown: " << nId << '>');
        }
    }

bool MapEvent::shouldDispatch(TypedHandle<const MapListener> /*vListener*/) const
    {
    return true;
    }

// ----- Describable interface ----------------------------------------------

String::View MapEvent::getDescription() const
    {
    switch (getId())
        {
        case entry_inserted:
            return COH_TO_STRING(super::getDescription() << " added: key=" << getKey() << ", value=" << getNewValue());

        case entry_updated:
            return COH_TO_STRING(super::getDescription() << " updated: key=" << getKey()
                << ", old value=" << getOldValue()
                << ", new value=" << getNewValue());

        case entry_deleted:
            return COH_TO_STRING(super::getDescription() << " deleted: key=" << getKey()
                << ", value=" << getOldValue());

        default:
            COH_THROW (IllegalStateException::create());
        }
    }


// ----- constants ----------------------------------------------------------

COH_EXPORT_SPEC_MEMBER(const int32_t MapEvent::entry_inserted)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEvent::entry_updated)
COH_EXPORT_SPEC_MEMBER(const int32_t MapEvent::entry_deleted)

COH_CLOSE_NAMESPACE2

