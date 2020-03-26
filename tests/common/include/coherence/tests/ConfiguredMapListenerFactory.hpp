/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONFIGURED_MAP_LISTENER_FACTORY_HPP
#define COH_CONFIGURED_MAP_LISTENER_FACTORY_HPP

#include "coherence/lang.ns"

#include "coherence/tests/ConfiguredMapListener.hpp"

#include "coherence/util/MapEvent.hpp"
#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

/**
* Factory methods called during configuration to instantiate
* ConfiguredMapListeners.
*/
class ConfiguredMapListenerFactory
    : public class_spec<ConfiguredMapListenerFactory,
        extends<Object> >
    {
    public:
        friend class factory<ConfiguredMapListenerFactory>;

    protected:
        ConfiguredMapListenerFactory();

    public:
        /**
        * Creates listener as instantiated factory method.
        */
        ConfiguredMapListener::Handle createConfiguredMapListenerFactoryInstance();

        /**
        * Creates listener as instantiated factory method with a parameter
        */
        ConfiguredMapListener::Handle createConfiguredMapListenerFactoryInstanceParams(String::View vsCacheName);

        /**
        * Creates listener as static factory method
        */
        static ConfiguredMapListener::Handle createConfiguredMapListenerFactoryStatic();

        /**
        * Creates listener as static factory method with a parameter
        */
        static ConfiguredMapListener::Handle createConfiguredMapListenerFactoryStaticParams(String::View vsCacheName);
    };

COH_CLOSE_NAMESPACE2

#endif
