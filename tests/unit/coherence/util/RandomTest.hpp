/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"
#include "coherence/lang.ns"

#include "coherence/util/Random.hpp"

using coherence::util::Random;

class RandomSuite : public CxxTest::TestSuite
    {
    public:
        void testRandomize()
            {
            ObjectArray::Handle ha = ObjectArray::create(7);
            ha[0] = COH_TO_STRING("Test0");
            ha[1] = COH_TO_STRING("Test1");
            ha[2] = COH_TO_STRING("Test2");
            ha[3] = COH_TO_STRING("Test3");
            ha[4] = COH_TO_STRING("Test4");
            ha[6] = COH_TO_STRING("Test5");

            ObjectArray::Handle hra = ObjectArray::copy(ha, 0, ObjectArray::create(ha->length));
            Random::Handle hRandom = Random::getInstance();

            hra = hRandom->randomize(hra);

            // ensure shallow randomization
            for (size32_t x = 0, c = ha->length; x < c; ++x)
                {
                for (size32_t y = 0, yc = hra->length; y < yc; ++y)
                    {
                    if (ha[x] == hra[y])
                        {
                        break;
                        }
                    else if (y == yc - 1)
                        {
                        TS_ASSERT(false);
                        }
                    }
                }

            // ensure some randomness
            bool fMismatch = false;
            for (size32_t i = 0; !fMismatch && i < 10; ++i)
                {
                for (size32_t x = 0, c = ha->length; x < c; ++x)
                    {
                    if (ha[x] != hra[x])
                        {
                        fMismatch = true;
                        break;
                        }
                    }
                if (!fMismatch)
                    {
                    // very unlikely, but we could have randomized to the same
                    // ordering, try a few times then give up
                    hra = hRandom->randomize(hra);
                    }
                }

            TS_ASSERT(fMismatch == true);
            }
    };
