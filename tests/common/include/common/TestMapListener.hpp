/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_MAP_LISTENER_HPP
#define COH_TEST_MAP_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(common, test)

using coherence::lang::Object;
using coherence::util::MapEvent;
using coherence::util::MapListener;

COH_OPEN_NAMESPACE_ANON(TestMapListener)

class EventHolder
    : public class_spec<EventHolder>
    {
    friend class factory<EventHolder>;

    // ----- constructor ------------------------------------------------

    protected:
        /**
         * Create a new EventHolder
         */
        EventHolder()
            : m_vsEvent(self())
            {
            }

    // ----- EventHolder methods --------------------------------------
    public:
        virtual String::View getEvent() const
            {
            return m_vsEvent;
            }

        virtual void setEventInsert()
            {
            m_vsEvent = String::create("insert");
            }

        virtual void setConfiguredEventInsert(String::View vsCacheName)
            {
            m_vsEvent = vsCacheName;
            }

        virtual void setEventUpdate()
            {
            m_vsEvent = String::create("update");
            }

        virtual void setEventDelete()
            {
            m_vsEvent = String::create("delete");
            }

    // ----- data members -----------------------------------------------

    protected:
        /**
        * event.
        */
        MemberView<String> m_vsEvent;
    };

/**
* Return the singleton EventHolder.
*
* @return the EventHolder
*/
EventHolder::Handle getEventHolder()
    {
    static FinalHandle<EventHolder> hSingleton(System::common(),
            EventHolder::create());
    return hSingleton;
    }
COH_STATIC_INIT(getEventHolder());

COH_CLOSE_NAMESPACE_ANON

class TestMapListener
    : public class_spec<TestMapListener,
        extends<Object>,
        implements<MapListener> >
    {
    public:
        friend class factory<TestMapListener>;

    public:
        TestMapListener(String::View vsCacheName)
            : m_vEvent(self()),
              m_hMonitor(self()),
              m_vsCacheName(self(), vsCacheName)
            {
            m_hMonitor = Object::create();
            }

    protected:
        TestMapListener()
            : m_vEvent(self()),
              m_hMonitor(self()),
              m_vsCacheName(self(), String::null_string)
            {
            m_hMonitor = Object::create();
            }

        TestMapListener(Object::Handle hMonitor)
            : m_vEvent(self()),
              m_hMonitor(self(), hMonitor),
              m_vsCacheName(self(), String::null_string)
            {
            }

    public:
        // ----- MapListener interface ------------------------------------

        virtual void entryInserted(MapEvent::View vEvent)
            {
            Object::Handle hObject     = m_hMonitor;
            String::View   vsCacheName = m_vsCacheName;
            COH_SYNCHRONIZED (hObject)
                {
                m_vEvent = vEvent;

                if (vsCacheName != NULL && vsCacheName->equals("configured-test"))
                    {
                    getEventHolder()->setConfiguredEventInsert(vsCacheName);
                    }
                else
                    {
                    getEventHolder()->setEventInsert();
                    }
                hObject->notifyAll();
                }
            }

        virtual void entryUpdated(MapEvent::View vEvent)
            {
            Object::Handle hObject = m_hMonitor;
            COH_SYNCHRONIZED (hObject)
                {
                m_vEvent = vEvent;
                getEventHolder()->setEventUpdate();
                hObject->notifyAll();
                }
            }

        virtual void entryDeleted(MapEvent::View vEvent)
            {
            Object::Handle hObject = m_hMonitor;
            COH_SYNCHRONIZED (hObject)
                {
                m_vEvent = vEvent;
                getEventHolder()->setEventDelete();
                hObject->notifyAll();
                }
            }

        // ----- object methods -------------------------------------------

        virtual size32_t hashCode() const
            {
            return Object::hashCode(m_vsCacheName);
            }

        virtual bool equals(Object::View vThat) const
            {
            if (instanceof<TestMapListener::View>(vThat))
                {
                TestMapListener::View vOther = cast<TestMapListener::View>(vThat);
                return Object::equals(m_vsCacheName, vOther->m_vsCacheName);
                }
            return false;
            }

        // ----- accessors ------------------------------------------------

        /**
        * @return the MapEvent received by this MapListener.
        */
        virtual MapEvent::View waitForEvent()
            {
            Object::Handle hObject = m_hMonitor;
            COH_SYNCHRONIZED (hObject)
                {
                MapEvent::View vEvent;
                if (NULL == (vEvent = m_vEvent))
                    {
                    try
                        {
                        hObject->wait(1000);
                        vEvent = m_vEvent;
                        }
                    catch (InterruptedException::View e)
                        {
                        Thread::currentThread()->interrupt();
                        COH_THROW (e);
                        }
                    }

                clearEvent();
                return vEvent;
                }
            }

        /**
        * Reset the MapEvent property.
        */
        virtual void clearEvent()
            {
            Object::Handle hObject = m_hMonitor;
            COH_SYNCHRONIZED (hObject)
                {
                m_vEvent = NULL;
                }
            }

        /**
        * Return the last event
        */
        static String::View getEvent()
            {
            return getEventHolder()->getEvent();
            }

        /**
        * Return the last m_vEvent
        */
        MapEvent::View getMapEvent()
            {
            return m_vEvent;
            }

        // ----- data members ---------------------------------------------

        /**
        * The last MapEvent received by this MapListener.
        */
        MemberView<MapEvent> m_vEvent;
        MemberHandle<Object> m_hMonitor;

        /**
        * Name of the cache configured on this TestListener.
        */
        MemberView<String>   m_vsCacheName;
    };

COH_REGISTER_CLASS(TypedClass<TestMapListener>::create()
        ->declare(COH_CONSTRUCTOR1(TestMapListener, String::View)));

COH_CLOSE_NAMESPACE2

#endif
