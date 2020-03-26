/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/security/auth/GenericSubject.hpp"

#include "private/coherence/security/SecurityHelper.hpp"

using namespace coherence::lang;

using coherence::security::SecurityHelper;
using coherence::security::auth::GenericSubject;
using coherence::security::auth::Subject;

/**
* Test Suite for the RunAs class.
*/
class RunAsTest : public CxxTest::TestSuite
    {
    public:
        void testRunAs()
            {
            Subject::View vSubjFoo = GenericSubject::create("foo");
            Subject::View vSubjBar = GenericSubject::create("bar");

            TS_ASSERT(SecurityHelper::getCurrentSubject() == NULL);
            COH_RUN_AS (vSubjFoo)
                {
                TS_ASSERT(SecurityHelper::getCurrentSubject() == vSubjFoo);
                COH_RUN_AS (vSubjBar)
                    {
                    TS_ASSERT(SecurityHelper::getCurrentSubject() == vSubjBar);
                    }
                TS_ASSERT(SecurityHelper::getCurrentSubject() == vSubjFoo);
                }
            TS_ASSERT(SecurityHelper::getCurrentSubject() == NULL);
            }
    };
