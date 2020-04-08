/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
/**
* Include common platform specific headers here - prior to including
* Coherence ns files
*/
#ifdef WIN32
# include<windows.h>
# include<windef.h>
# include<winsock.h>
#endif

#include "cxxtest/TestSuite.h"

/**
* Include all public Coherence ns files here
*/
#include<coherence/io/pof/reflect.ns>
#include<coherence/io/pof.ns>
#include<coherence/io.ns>
#include<coherence/lang.ns>
#include<coherence/native.ns>
#include<coherence/net/cache.ns>
#include<coherence/net.ns>
#include<coherence/run/xml.ns>
#include<coherence/run.ns>
#include<coherence/security/auth.ns>
#include<coherence/security.ns>
#include<coherence/stl.ns>
#include<coherence/util/aggregator.ns>
#include<coherence/util/comparator.ns>
#include<coherence/util/extractor.ns>
#include<coherence/util/filter.ns>
#include<coherence/util/processor.ns>
#include<coherence/util/transformer.ns>
#include<coherence/util.ns>

/**
* Compilation test to verify that commonly included platform specific
* headers do not have name collisions with enums, consts, macros, etc.,
* in Coherence header files.
*/
class PlatformTest : public CxxTest::TestSuite
    {
    public:
        void testDoNothing()
            {
            TS_ASSERT(true);
            }
    };
