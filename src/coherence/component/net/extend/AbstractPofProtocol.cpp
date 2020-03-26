/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/component/net/extend/AbstractPofProtocol.hpp"

#include "coherence/util/HashMap.hpp"

COH_OPEN_NAMESPACE4(coherence,component,net,extend)

using coherence::util::HashMap;


// ----- constructors -------------------------------------------------------

AbstractPofProtocol::AbstractPofProtocol()
    : f_hMapMessageFactory(self(), Map::Handle(NULL), true)
    {
    }


// ----- Protocol interface -------------------------------------------------

Protocol::MessageFactory::View AbstractPofProtocol::getMessageFactory(
        int32_t nVersion) const
    {
    COH_SYNCHRONIZED (this)
        {
        if (nVersion < getSupportedVersion() || nVersion > getCurrentVersion())
            {
            COH_THROW_STREAM (IllegalArgumentException,
                    "protocol " << getName() << " does not support version " << nVersion);
            }

        Map::Handle hMap = f_hMapMessageFactory;
        if (NULL == hMap)
            {
            hMap = HashMap::create();
            initialize(f_hMapMessageFactory, hMap);
            }

        Integer32::View vIVersion = Integer32::valueOf(nVersion);

        AbstractPofMessageFactory::Handle hFactory  =
                cast<AbstractPofMessageFactory::Handle>(hMap->get(vIVersion));
        if (NULL == hFactory)
            {
            hFactory = instantiateMessageFactory(nVersion);
            hFactory->setProtocol(this);
            hFactory->setVersion(nVersion);

            COH_ENSURE_EQUALITY(hFactory->getVersion(), nVersion);
            hMap->put(vIVersion, hFactory);
            }

        return hFactory;
        }
    }


// ----- Describable interface ----------------------------------------------

String::View AbstractPofProtocol::getDescription() const
    {
    return COH_TO_STRING("Versions=[" << getSupportedVersion()
        << ".." << getCurrentVersion() << ']');
    }

COH_CLOSE_NAMESPACE4
