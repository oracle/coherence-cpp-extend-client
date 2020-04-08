/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "coherence/tests/ConfiguredMapListener.hpp"
#include "coherence/tests/ConfiguredMapListenerFactory.hpp"

#include "coherence/util/MapListener.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

ConfiguredMapListenerFactory::ConfiguredMapListenerFactory()
   {}

ConfiguredMapListener::Handle ConfiguredMapListenerFactory::createConfiguredMapListenerFactoryInstance()
   {
   return ConfiguredMapListener::create("createConfiguredMapListenerFactoryInstance");
   }

ConfiguredMapListener::Handle ConfiguredMapListenerFactory::createConfiguredMapListenerFactoryInstanceParams(String::View vsCacheName)
   {
   return ConfiguredMapListener::create(vsCacheName);
   }

ConfiguredMapListener::Handle ConfiguredMapListenerFactory::createConfiguredMapListenerFactoryStatic()
   {
   return ConfiguredMapListener::create("createConfiguredMapListenerFactoryStatic");
   }

ConfiguredMapListener::Handle ConfiguredMapListenerFactory::createConfiguredMapListenerFactoryStaticParams(String::View vsCacheName)
   {
   return ConfiguredMapListener::create(vsCacheName);
   }

COH_REGISTER_TYPED_CLASS(ConfiguredMapListenerFactory);
COH_REGISTER_METHOD(ConfiguredMapListenerFactory, COH_METHOD1(ConfiguredMapListener::Handle, ConfiguredMapListenerFactory::Handle, createConfiguredMapListenerFactoryInstanceParams, String::View));
COH_REGISTER_METHOD(ConfiguredMapListenerFactory, COH_METHOD0(ConfiguredMapListener::Handle, ConfiguredMapListenerFactory::Handle, createConfiguredMapListenerFactoryInstance));
COH_REGISTER_METHOD(ConfiguredMapListenerFactory, COH_STATIC_METHOD1(ConfiguredMapListener::Handle, ConfiguredMapListenerFactory, createConfiguredMapListenerFactoryStaticParams, String::View));
COH_REGISTER_METHOD(ConfiguredMapListenerFactory, COH_STATIC_METHOD0(ConfiguredMapListener::Handle, ConfiguredMapListenerFactory, createConfiguredMapListenerFactoryStatic));

COH_CLOSE_NAMESPACE2
