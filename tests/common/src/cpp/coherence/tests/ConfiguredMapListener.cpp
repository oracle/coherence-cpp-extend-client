/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "coherence/tests/ConfiguredMapListener.hpp"
#include "coherence/lang/Thread.hpp"
#include "coherence/lang/InterruptedException.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

COH_OPEN_NAMESPACE_ANON(ConfiguredMapListener)

/**
* Class used to hold an event until it is requested.
*/
class TestEventHolder
    : public class_spec<TestEventHolder>
    {
    friend class factory<TestEventHolder>;

    // ----- constructor ------------------------------------------------

    protected:
        /**
         * Create a new EventHolder
         */
        TestEventHolder()
            : m_vsEvent(self())
            {
            }

    // ----- EventHolder methods --------------------------------------

    public:
        virtual String::View getEvent() const
            {
            if (m_vsEvent == NULL)
                {
                try
                    {
                    Thread::sleep(1000);
                    }
                catch (InterruptedException::View e)
                    {
                    }
                }
            return m_vsEvent;
            }

        /**
        * Reset the MapEvent property.
        */
        void clearEvent()
            {
            m_vsEvent = NULL;
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

COH_CLOSE_NAMESPACE_ANON

namespace
    {
    /**
    * Return the singleton TestEventHolder.
    *
    * @return the TestEventHolder
    */
    TestEventHolder::Handle getTestEventHolder()
        {
        static FinalHandle<TestEventHolder> hSingle(System::common(),
                TestEventHolder::create());
        return hSingle;
        }
    COH_STATIC_INIT(getTestEventHolder());
    }

// ----- Constructors ----------------------------------------------------------

ConfiguredMapListener::ConfiguredMapListener(String::View vsCacheName)
        : f_vsCacheName(self(), vsCacheName)
    {
    }

ConfiguredMapListener::ConfiguredMapListener()
        : f_vsCacheName(self(), String::create("ConfiguredMapListenerInstanceNoParameters"))
    {
    }

// ----- MapListener interface ------------------------------------------------

void ConfiguredMapListener::entryInserted(MapEvent::View /*vEvent*/)
    {
    getTestEventHolder()->setConfiguredEventInsert(f_vsCacheName);
    }

void ConfiguredMapListener::entryUpdated(MapEvent::View /*vEvent*/)
    {
    getTestEventHolder()->setEventUpdate();
    }

void ConfiguredMapListener::entryDeleted(MapEvent::View /*vEvent*/)
    {
    getTestEventHolder()->setEventDelete();
    }

// ----- accessors ----------------------------------------------------------

String::View ConfiguredMapListener::getEvent()
    {
    return getTestEventHolder()->getEvent();
    }

void ConfiguredMapListener::clearEvent()
    {
    getTestEventHolder()->clearEvent();
    }

COH_REGISTER_CLASS(TypedClass<ConfiguredMapListener>::create()
        ->declare(COH_CONSTRUCTOR1(ConfiguredMapListener, String::View)));

COH_CLOSE_NAMESPACE2


