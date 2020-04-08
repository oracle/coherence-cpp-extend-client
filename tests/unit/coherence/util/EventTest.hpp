/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"
#include "coherence/util/Event.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::util::Event;


/**
* Test suite for the Event class. 
*/
class EventTest : public CxxTest::TestSuite
    {
    public:
        /** 
        * Test Event creation.  
        */
        void testCreate()
            {
            String::View  vsSource = "SOURCE";
            Event::Handle hEvt     = Event::create(vsSource);
            TS_ASSERT(hEvt->getSource() == vsSource);
            }
    };
