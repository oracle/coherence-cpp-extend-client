/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONFIGURED_MAP_LISTENER_HPP
#define COH_CONFIGURED_MAP_LISTENER_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using namespace coherence::lang;

using coherence::util::MapEvent;
using coherence::util::MapListener;

/**
* Test MapListener class used to test
* configured MapListener.
*/
class ConfiguredMapListener
    : public class_spec<ConfiguredMapListener,
        extends<Object>,
        implements<MapListener> >
    {
    public:
        friend class factory<ConfiguredMapListener>;

    public:
        /**
        * Constructor - for storing cache name
        */
        ConfiguredMapListener(String::View vsCacheName);

    protected:
        /**
        * Default Constructor
        */
        ConfiguredMapListener();

    public:        
        // ----- MapListener interface ------------------------------------

        /**
        * {@inheritDoc}
        */
        virtual void entryInserted(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryUpdated(MapEvent::View vEvent);

        /**
        * {@inheritDoc}
        */
        virtual void entryDeleted(MapEvent::View vEvent);

        // ----- accessors ------------------------------------------------

        /**
        * Return the last event
        */
        static String::View getEvent();

        /**
        * Clear the last event
        */
        static void clearEvent();

        // ----- data members ---------------------------------------------

        /**
        * Name of the cache configured on this TestListener.
        */
        FinalView<String> f_vsCacheName;
    };

COH_CLOSE_NAMESPACE2

#endif
