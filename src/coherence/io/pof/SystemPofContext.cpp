/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/HashMap.hpp"

#include <iostream>

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::util::HashMap;

COH_REGISTER_CLASS((TypedExecutableClass<SystemPofContext,
        TypedBarrenClass<SystemPofContext> >::create()));


// ----- constructors -------------------------------------------------------

SystemPofContext::SystemPofContext()
    : super()
    {
    }

SystemPofContext::SystemPofContext(const SystemPofContext& that)
    : super(that)
    {
    }


// ----- static methods -----------------------------------------------------

SystemPofContext::Handle SystemPofContext::getInstance()
    {
    static FinalHandle<SystemPofContext> hCtx
        (System::common(), create());
    return hCtx;
    }
COH_STATIC_INIT(SystemPofContext::getInstance());

void SystemPofContext::main(ObjectArray::View vasArg)
    {
    if (vasArg->length == 0)
        {
        std::cout << getInstance()->f_hMapClassName << std::endl;
        }
    else
        {
        Map::View   vMapLoaded = getInstance()->f_hMapClassName;
        Map::Handle hMapStart  = HashMap::create();
        hMapStart->putAll(vMapLoaded);

        for (size32_t i = 0, c = vasArg->length; i < c; ++i)
            {
            System::loadLibrary(cast<String::View>(vasArg[i]));

            Map::Handle hMapEnd = HashMap::create();
            hMapEnd->putAll(vMapLoaded);
            hMapEnd->entrySet()->removeAll(hMapStart->entrySet());

            if (i > 0)
                {
                std::cout << std::endl;
                }

            std::cout << "SystemPofContext: " << vasArg[i]
                << " registered classes\n"
                << hMapEnd << std::endl;
            }
        }
    }

COH_CLOSE_NAMESPACE3
