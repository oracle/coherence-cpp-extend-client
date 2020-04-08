/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ExecutableClass.hpp"

#include "coherence/lang/IllegalArgumentException.hpp"
#include "coherence/lang/Method.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/SystemClassLoader.hpp"

using namespace coherence::lang;

/**
* Helper launcher function to be used by stand-alone executable which loads
* the Coherence library as a dynamic library for the purpose of executing
* a managed class.
*/
extern "C" COH_EXPORT int coh_execute_class(int argc, char** argv)
    {
    try
        {
        COH_ENSURE_PARAM_RELATION(argc, >=, 1);

        Class::View vClass = SystemClassLoader::getInstance()->loadByName(argv[0]);

        ObjectArray::Handle hasArg = ObjectArray::create(argc - 1);
        for (int i = 1; i < argc; ++i)
            {
            hasArg[i - 1] = String::create(argv[i]);
            }

        if (instanceof<ExecutableClass::View>(vClass))
            {
            cast<ExecutableClass::View>(vClass)->execute(hasArg);
            }
        else // use reflection
            {
            ObjectArray::Handle ha = ObjectArray::create(1);
            ha[0] = hasArg;
            vClass->getDeclaredMethod("main", ha, Method::modifier_static)
                    ->invoke(NULL, ha);
            }
        }
    catch (const std::exception& e)
        {
        std::cerr << e.what() << std::endl;
        // no throw
        return 1;
        }
    return 0;
    }
