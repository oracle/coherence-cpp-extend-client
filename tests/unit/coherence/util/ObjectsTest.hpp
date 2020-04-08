/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Objects.hpp"

using namespace coherence::lang;

using coherence::util::Comparator;
using coherence::util::Objects;

/**
 * Test Suite for the Objects static methods.
 */
class ObjectsTest : public CxxTest::TestSuite
    {

    // ----- inner class: HashComparator -------------------------------------

    /**
     * Compare two objects based using their hash values.
     */
    class HashComparator
        : public class_spec<HashComparator,
            extends<Object>,
            implements<Comparator> >
        {
        friend class factory<HashComparator>;

        // ----- Singleton ---------------------------------------------------

        public:
            static HashComparator::View instance()
                {
                static HashComparator::View vSingleton = HashComparator::create();
                return vSingleton;
                }

        // ----- Comparator interface ----------------------------------------

        public:
            /*
             * @inheritDoc
             */
            virtual int32_t compare(Object::View v1, Object::View v2) const
                {
                size32_t nV1Hash = Objects::hashCode(v1);
                size32_t nV2Hash = Objects::hashCode(v2);
                return nV1Hash == nV2Hash ? 0 : nV1Hash < nV2Hash ? -1 : 1;
                }
        };

    public:
        void testToString()
            {
            String::View vsFoo     = "foo";
            String::View vsDefault = "default";

            TS_ASSERT(Objects::toString(vsFoo, vsDefault)->equals(vsFoo));
            TS_ASSERT(Objects::toString(NULL,  vsDefault)->equals(vsDefault));

            TS_ASSERT(Objects::toString(Integer32::create(1), vsDefault)->equals("1"));
            }

        void testCompare()
            {
            HashComparator::View vComparator = HashComparator::instance();
            String::View         vsBar       = "bar";
            String::View         vsFoo       = "foo";

            TS_ASSERT_EQUALS(Objects::compare(NULL,  NULL,  vComparator),  0);
            TS_ASSERT_EQUALS(Objects::compare(vsFoo, NULL,  vComparator),  1);
            TS_ASSERT_EQUALS(Objects::compare(NULL,  vsFoo, vComparator), -1);
            TS_ASSERT_EQUALS(Objects::compare(vsFoo, vsFoo, vComparator),  0);
            TS_ASSERT_EQUALS(Objects::compare(vsFoo, vsBar, vComparator),  1);
            TS_ASSERT_EQUALS(Objects::compare(vsBar, vsFoo, vComparator), -1);
            }

        void testRequireNonNull()
            {
            String::View vsFoo = "foo";

            TS_ASSERT_EQUALS(Objects::requireNonNull(vsFoo), vsFoo);
            TS_ASSERT_EQUALS(Objects::requireNonNull(vsFoo, vsFoo), vsFoo);
            TS_ASSERT_THROWS(Objects::requireNonNull(NULL), NullPointerException::View);
            TS_ASSERT_THROWS_ASSERT(Objects::requireNonNull(NULL, vsFoo),
                    NullPointerException::View vE,
                    vE->getMessage()->equals(vsFoo));
            }

        void testIsNull()
            {
            TS_ASSERT( Objects::isNull(NULL));
            TS_ASSERT(!Objects::isNull(String::create("foo")));
            }

        void testNonNull()
            {
            TS_ASSERT(!Objects::nonNull(NULL));
            TS_ASSERT( Objects::nonNull(String::create("foo")));
            }
    };
