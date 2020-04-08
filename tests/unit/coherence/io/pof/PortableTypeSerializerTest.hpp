/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"

#include "coherence/io/pof/PortableObjectSerializer.hpp"
#include "coherence/io/pof/PortableTypeSerializer.hpp"
#include "coherence/io/pof/SimplePofContext.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/Binary.hpp"
#include "coherence/util/SerializationHelper.hpp"

#include "common/Color.hpp"

#include "common/v1/Dog.hpp"
#include "common/v1/Pet.hpp"

#include "common/v2/Animal.hpp"
#include "common/v2/Dog.hpp"
#include "common/v2/Pet.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;

using coherence::io::Serializer;
using coherence::io::pof::PortableObjectSerializer;
using coherence::io::pof::PortableTypeSerializer;
using coherence::io::pof::SimplePofContext;
using coherence::io::pof::SystemPofContext;
using coherence::util::Binary;
using coherence::util::SerializationHelper;

using common::test::Color;

/**
 * Test suite for the PortableTypeSerializer.
 */
class PortableTypeSerializerTest : public CxxTest::TestSuite
    {
    public:
        void setUp()
            {
            // CxxTest runs setUp() before each individual test.  We just want this to run
            // once for this test suite.
            if (NULL == vV1Ser)
                {
                try
                    {
                    ClassLoader::View vLoader = SystemClassLoader::getInstance();

                    SimplePofContext::Handle hCtx = SimplePofContext::create();
                    hCtx->registerUserType(common::test::v1::Pet::s_nTypeId,
                            vLoader->loadByName("common::test::v1::Pet"),
                            PortableTypeSerializer::create(common::test::v1::Pet::s_nTypeId));
                    hCtx->registerUserType(common::test::v1::Dog::s_nTypeId,
                            vLoader->loadByName("common::test::v1::Dog"),
                            PortableTypeSerializer::create(common::test::v1::Dog::s_nTypeId));
                    vV1Ser = hCtx;

                    hCtx = SimplePofContext::create();
                    hCtx->registerUserType(common::test::v2::Animal::s_nTypeId,
                            vLoader->loadByName("common::test::v2::Animal"),
                            PortableTypeSerializer::create(common::test::v2::Animal::s_nTypeId));
                    hCtx->registerUserType(common::test::v2::Pet::s_nTypeId,
                            vLoader->loadByName("common::test::v2::Pet"),
                            PortableTypeSerializer::create(common::test::v2::Pet::s_nTypeId));
                    hCtx->registerUserType(common::test::v2::Dog::s_nTypeId,
                            vLoader->loadByName("common::test::v2::Dog"),
                            PortableTypeSerializer::create(common::test::v2::Dog::s_nTypeId));
                    hCtx->registerUserType(5, vLoader->loadByName("common::test::Color"),
                            PortableObjectSerializer::create(5));
                    vV2Ser = hCtx;
                    }
                catch (Exception::View ve)
                    {
                    // display exception as CxxTest does not
                    std::cout << ve->getMessage() << std::endl << ve->getStackTrace() << std::endl;
                    COH_THROW(ve);
                    }
                }
            }

        void testRoundTripV1()
            {
            common::test::v1::Dog::View vDog = common::test::v1::Dog::create("Nadia", "Boxer");
            Binary::View vBinDog = SerializationHelper::toBinary(vDog, vV1Ser);
            TS_ASSERT(vDog->equals(SerializationHelper::fromBinary(vBinDog, vV1Ser)));
            }

        void testRoundTripV2()
            {
            common::test::v2::Dog::View vDog =
                    common::test::v2::Dog::create("Nadia", 10, "Boxer", Color::BRINDLE());
            Binary::View vBinDog = SerializationHelper::toBinary(vDog, vV2Ser);
            TS_ASSERT(vDog->equals(SerializationHelper::fromBinary(vBinDog, vV2Ser)));
            }

        void testEvolution()
            {
            common::test::v2::Dog::View vDogV2 =
                    common::test::v2::Dog::create("Nadia", 10, "Boxer", Color::BRINDLE());
            Binary::View vBinDogV2 = SerializationHelper::toBinary(vDogV2, vV2Ser);

            common::test::v1::Dog::View vDogV1 =
                    cast<common::test::v1::Dog::View>(SerializationHelper::fromBinary(vBinDogV2, vV1Ser));
            Binary::View vBinDogV1 = SerializationHelper::toBinary(vDogV1, vV1Ser);

            TS_ASSERT(vDogV2->equals(SerializationHelper::fromBinary(vBinDogV1, vV2Ser)));
            }

        void testSystemPofContextRegistration()
            {
            Serializer::View vSystemSer = SystemPofContext::getInstance();
            common::test::v2::Dog::View vDogV2 =
                    common::test::v2::Dog::create("Nadia", 10, "Boxer", Color::BRINDLE());
            Binary::View vBinDogV2 = SerializationHelper::toBinary(vDogV2, vSystemSer);

            common::test::v1::Dog::View vDogV1 =
                    cast<common::test::v1::Dog::View>(SerializationHelper::fromBinary(vBinDogV2, vV1Ser));
            Binary::View vBinDogV1 = SerializationHelper::toBinary(vDogV1, vV1Ser);

            TS_ASSERT(vDogV2->equals(SerializationHelper::fromBinary(vBinDogV1, vSystemSer)));
            }

    // ---- data members ----------------------------------------------------

    private:
        /**
         * Serializer with v1 classes registered.
         */
        Serializer::View vV1Ser;

        /**
         * Serializer with v2 classes registered.
         */
        Serializer::View vV2Ser;
    };
