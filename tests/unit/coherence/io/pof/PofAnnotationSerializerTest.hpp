/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "PofAnnotationObjects.hpp"

#include "coherence/io/pof/PofAnnotationSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/util/Binary.hpp"
#include "coherence/util/SerializationHelper.hpp"

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using annotations_test::BewilderedPerson;
using annotations_test::Child;
using annotations_test::Father;
using annotations_test::GrandFather;
using annotations_test::Person;
using annotations_test::PersonV1;
using annotations_test::PersonV2;
using annotations_test::Poffed;

using coherence::io::pof::PofAnnotationSerializer;
using coherence::io::pof::PofSerializer;
using coherence::io::pof::SimplePofContext;
using coherence::io::pof::SystemPofContext;

using coherence::io::pof::annotation::PortableProperty;

using coherence::util::SerializationHelper;
using coherence::util::Binary;

/**
* Test suite for PofAnnotationSerializer.
*/
class PofAnnotationSerializerTest : public CxxTest::TestSuite
    {
    private:
        typedef PofAnnotationSerializer PAS;
        typedef SerializationHelper     EH;

    public:
        /**
         * Test serialization.
         */
        void testSerialization()
            {
            ClassLoader::View        vLoader = SystemClassLoader::getInstance();
            Class::View              vClass  = vLoader->loadByType(typeid(Poffed));
            Class::View              vChildClass  = vLoader->loadByType(typeid(Child));
            SimplePofContext::Handle hCxt    = SimplePofContext::create();
            SystemPofContext::Handle hSysCxt = SystemPofContext::getInstance();

            PofSerializer::View vChildSerializer = cast<PofSerializer::View>(PAS::create(1004,vChildClass,true));

            hCxt->registerUserType(1004, vChildClass, vChildSerializer);

            Poffed::Handle hPerson = Poffed::create("Frank", "Spencer", 57);
            Child::Handle  hChild  = Child ::create("Betty", "Spencer", 55);

            Binary::View   vBinPerson  = EH::toBinary(hPerson, hSysCxt);
            Poffed::Handle hTeleported = cast<Poffed::Handle>(EH::fromBinary(vBinPerson, hSysCxt));
            Child::Handle hTeleported2 = cast<Child::Handle>(EH::fromBinary( EH::toBinary(hChild, hCxt), hCxt));

            TS_ASSERT(hTeleported->getFirstName()->equals("Frank"));
            TS_ASSERT(hTeleported->getLastName()->equals("Spencer"));
            TS_ASSERT(hTeleported->getAge()==57);
            TS_ASSERT(hTeleported2->getFirstName()->equals("Betty"));
            TS_ASSERT(hTeleported2->getLastName()->equals("Spencer"));
            TS_ASSERT(hTeleported2->getAge()==55);
            }

        /**
         * Test nested objects.
         */
        void testAncestry()
            {
            ClassLoader::View        vLoader = SystemClassLoader::getInstance();
            Class::View              vChildClass   = vLoader->loadByType(typeid(Child));
            Class::View              vFatherClass  = vLoader->loadByType(typeid(Father));
            Class::View              vGFatherClass = vLoader->loadByType(typeid(GrandFather));
            SimplePofContext::Handle hCxt    = SimplePofContext::create();

            PofSerializer::View vGFatherSerializer = cast<PofSerializer::View>(PAS::create(1002, vGFatherClass, true));
            PofSerializer::View vFatherSerializer  = cast<PofSerializer::View>(PAS::create(1003, vFatherClass, true));
            PofSerializer::View vChildSerializer   = cast<PofSerializer::View>(PAS::create(1004, vChildClass, true));

            hCxt->registerUserType(1002, vGFatherClass, vGFatherSerializer);
            hCxt->registerUserType(1003, vFatherClass , vFatherSerializer);
            hCxt->registerUserType(1004, vChildClass  , vChildSerializer);

            Child::Handle       hSon  = Child ::create("Bart", "Simpson", 10);
            Father::Handle      hDad  = Father::create("Homer", "Simpson", 50, hSon);
            GrandFather::Handle hGF   = GrandFather::create("Abe", "Simpson", 100, hDad);

            GrandFather::Handle hTeleported = cast<GrandFather::Handle>(EH::fromBinary( EH::toBinary(hGF, hCxt), hCxt));

            TS_ASSERT(hTeleported->getFirstName()->equals("Abe"));
            TS_ASSERT(hTeleported->getLastName()->equals("Simpson"));
            TS_ASSERT(hTeleported->getAge()==100);
            TS_ASSERT(hTeleported->getFather()->getFirstName()->equals("Homer"));
            TS_ASSERT(hTeleported->getFather()->getLastName()->equals("Simpson"));
            TS_ASSERT(hTeleported->getFather()->getAge()==50);
            TS_ASSERT(hTeleported->getFather()->getChild()->getFirstName()->equals("Bart"));
            TS_ASSERT(hTeleported->getFather()->getChild()->getLastName()->equals("Simpson"));
            TS_ASSERT(hTeleported->getFather()->getChild()->getAge()==10);
            }

        /**
         * Test inheritance.
         */
        void testInheritance()
            {
            ClassLoader::View        vLoader = SystemClassLoader::getInstance();
            Class::View              vPersonClass  = vLoader->loadByType(typeid(Person));
            Class::View              vBewilderedPersonClass  = vLoader->loadByType(typeid(BewilderedPerson));
            SimplePofContext::Handle hCxt    = SimplePofContext::create();

            PofSerializer::View vPersonSerializer = cast<PofSerializer::View>(PAS::create(1001, vPersonClass, true));
            PofSerializer::View vBewilderedPersonSerializer  = cast<PofSerializer::View>(PAS::create(1005, vBewilderedPersonClass, true));

            hCxt->registerUserType(1001, vPersonClass, vPersonSerializer);
            hCxt->registerUserType(1005, vBewilderedPersonClass , vBewilderedPersonSerializer);

            BewilderedPerson::Handle hPerson = BewilderedPerson::create("Frank", "Spencer", 57, "dizzy");
            BewilderedPerson::Handle hTeleported = cast<BewilderedPerson::Handle>(EH::fromBinary( EH::toBinary(hPerson, hCxt), hCxt));

            TS_ASSERT(hTeleported->getFirstName()->equals("Frank"));
            TS_ASSERT(hTeleported->getLastName()->equals("Spencer"));
            TS_ASSERT(hTeleported->getAge()==57);
            TS_ASSERT(hTeleported->getState()->equals("dizzy"));
            }

        /**
         * Test evolvability support.
         */
        void testEvolvable()
            {
            ClassLoader::View        vLoader        = SystemClassLoader::getInstance();
            Class::View              vPersonV1Class = vLoader->loadByType(typeid(PersonV1));
            Class::View              vPersonV2Class = vLoader->loadByType(typeid(PersonV2));
            SimplePofContext::Handle hCtx1          = SimplePofContext::create();
            SimplePofContext::Handle hCtx2          = SimplePofContext::create();

            PofSerializer::View vPersonV1Serializer = cast<PofSerializer::View>(PAS::create(1001, vPersonV1Class, true));
            PofSerializer::View vPersonV2Serializer = cast<PofSerializer::View>(PAS::create(1001, vPersonV2Class, true));
            hCtx1->registerUserType(1001, vPersonV1Class, vPersonV1Serializer);
            hCtx2->registerUserType(1001, vPersonV2Class, vPersonV2Serializer);

            PersonV1::Handle hPersonV1 = PersonV1::create("Frank", "Spencer", 57);

            // verify we can go forward 1 => 2
            PersonV2::Handle hTeleportedV2 = cast<PersonV2::Handle>(EH::fromBinary(EH::toBinary(hPersonV1, hCtx1), hCtx2));

            // verify we can go back 2 => 1
            hTeleportedV2->setMale(Boolean::valueOf(true));
            PersonV1::Handle hTeleportedV1 = cast<PersonV1::Handle>(EH::fromBinary(EH::toBinary(hTeleportedV2, hCtx2), hCtx1));

            PersonV2::Handle hTeleportedV2FromV1 = cast<PersonV2::Handle>(EH::fromBinary(EH::toBinary(hTeleportedV1, hCtx1), hCtx2));

            // v1 => v2
            TS_ASSERT(hTeleportedV2->getFirstName()->equals("Frank"));
            TS_ASSERT(hTeleportedV2->getLastName()->equals("Spencer"));
            TS_ASSERT(hTeleportedV2->getAge()==57);

            // v2 => v1
            TS_ASSERT(NULL != hTeleportedV1->getFutureData());
            TS_ASSERT(hTeleportedV1->getFirstName()->equals(hPersonV1->getFirstName()));
            TS_ASSERT(hTeleportedV1->getLastName()->equals(hPersonV1->getLastName()));
            TS_ASSERT_EQUALS(hTeleportedV1->getAge(), hPersonV1->getAge());
            TS_ASSERT(hTeleportedV2FromV1->getFirstName()->equals(hPersonV1->getFirstName()));
            TS_ASSERT(hTeleportedV2FromV1->getLastName()->equals(hPersonV1->getLastName()));
            TS_ASSERT_EQUALS(hTeleportedV2FromV1->getAge(), hPersonV1->getAge());
            TS_ASSERT(hTeleportedV2FromV1->isMale()->getValue());
            }
    };
