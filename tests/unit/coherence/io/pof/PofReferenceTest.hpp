/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayWriteBuffer.hpp"
#include "coherence/io/pof/PofBufferReader.hpp"
#include "coherence/io/pof/PofBufferWriter.hpp"
#include "coherence/io/pof/SimplePofContext.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"
#include "coherence/util/CompositeKey.hpp"
#include "coherence/util/Iterator.hpp"

#include "common/EvolvablePortablePerson.hpp"
#include "common/EvolvablePortablePerson2.hpp"
#include "common/NestedTypeWithReference.hpp"
#include "common/PortablePerson.hpp"
#include "common/PortablePersonReference.hpp"
#include "common/SimpleType.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;
using namespace std;
using namespace common::test;

using coherence::io::OctetArrayWriteBuffer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofBufferWriter;
using coherence::io::pof::SimplePofContext;
using coherence::io::pof::SystemPofContext;
using coherence::util::Collection;
using coherence::util::CompositeKey;
using coherence::util::Iterator;

using common::test::EvolvablePortablePerson;
using common::test::EvolvablePortablePerson2;
using common::test::NestedTypeWithReference;
using common::test::PortablePerson;
using common::test::PortablePersonReference;


/**
 * Test suite for POF methods that work with arrays.
 */
class PofReferenceTest: public CxxTest::TestSuite
    {
    private:
        OctetArrayWriteBuffer::Handle m_hBuf;
        SystemPofContext::Handle      m_hCtx;
        PofBufferWriter::Handle       m_hWriter;
        PofBufferReader::Handle       m_hReader;

        void initPofWriter()
            {
            m_hBuf = OctetArrayWriteBuffer::create(1024);
            m_hCtx = SystemPofContext::getInstance();
            m_hWriter = PofBufferWriter::create(m_hBuf->getBufferOutput(), m_hCtx);
            m_hWriter->enableReference();
            }

        void initPofReader()
            {
            m_hReader = PofBufferReader::create(
                    m_hBuf->getReadBuffer()->getBufferInput(), m_hCtx);
            }

    public:
        /**
         * Test the PofBufferWriter::writeObjectArray() method with uniform
         * object array.
         */
        void testDuplicateObjectReferences()
            {
            SystemClassLoader::Handle hLoader =
                    SystemClassLoader::getInstance();
            initPofWriter();
            hLoader->registerClass(
                    coherence::lang::TypedClass<Address>::create());
            hLoader->registerClass(coherence::lang::TypedClass<
                    EvolvablePortablePerson>::create());
            hLoader->registerClass(coherence::lang::TypedClass<
                    EvolvablePortablePerson2>::create());
            hLoader->registerClass(coherence::lang::TypedClass<
                    PortablePersonReference>::create());
            m_hCtx->registerUserType(1001, hLoader->loadByType(typeid(Address)),
                    PortableObjectSerializer::create(1001));
            m_hCtx->registerUserType(1002, hLoader->loadByType(
                    typeid(PortablePerson)), PortableObjectSerializer::create(
                    1002));
            m_hCtx->registerUserType(1003, hLoader->loadByType(
                    typeid(EvolvablePortablePerson)),
                    PortableObjectSerializer::create(1003));
            m_hCtx->registerUserType(1004, hLoader->loadByType(
                    typeid(EvolvablePortablePerson2)),
                    PortableObjectSerializer::create(1004));
            m_hCtx->registerUserType(1005, hLoader->loadByType(
                    typeid(PortablePersonReference)),
                    PortableObjectSerializer::create(1005));
            m_hCtx->registerUserType(1010, hLoader->loadByType(
                    typeid(CompositeKey)), PortableObjectSerializer::create(
                    1010));
            m_hCtx->setReferenceEnabled(true);

            PortablePerson::Handle hJoe          = PortablePerson::create(
                    String::create("Joe Smith"), 30);
            PortablePerson::Handle hDifferentJoe = PortablePerson::create(
                    String::create("Joe Smith"), 30);
            CompositeKey::Handle   hKey          = CompositeKey::create(hJoe, hJoe);
            PortablePerson::Handle hJoan         = PortablePerson::create(
                    String::create("Joan Smith"), 28);
            PortablePerson::Handle hJay          = PortablePerson::create(
                    String::create("Jay Smith"), 5);
            Address::Handle        hAddr         = Address::create(String::create(
                    "One Oracle Drive"), String::create("Nashua"),
                    String::create("NH"), String::create("03062"));
            ObjectArray::Handle    haChildren    = ObjectArray::create(1);

            TS_ASSERT(hKey->getPrimaryKey() == hKey->getSecondaryKey());
            hJoe->setAddress(hAddr);
            hJoe->setSpouse(hJoan);
            haChildren[0] = hJay;
            hJoe->setChildren(cast<ObjectArray::View> (haChildren));

            hDifferentJoe->setAddress(hAddr);
            hDifferentJoe->setSpouse(hJoan);
            hDifferentJoe->setChildren(cast<ObjectArray::View> (haChildren));

            m_hWriter->writeObject(-1, hKey);
            initPofReader();
            CompositeKey::View   vKeyR = cast<CompositeKey::View> (
                    m_hReader->readObject(-1));
            TS_ASSERT(hKey->getPrimaryKey()->equals(vKeyR->getPrimaryKey()));

            PortablePerson::View vP1R  = cast<PortablePerson::View> (
                    vKeyR->getPrimaryKey());
            PortablePerson::View vP2R  = cast<PortablePerson::View> (
                    vKeyR->getSecondaryKey());
            TS_ASSERT(vP1R->getAddress()->equals(vP2R->getAddress()));
            TS_ASSERT(vP1R->equals(vP2R));
            TS_ASSERT(vKeyR->getPrimaryKey() == vKeyR->getSecondaryKey());

            // test a collection of duplicate object references
            Collection::Handle hCol = ArrayList::create();
            hCol->add(hJoe);
            hCol->add(hJoe);
            hCol->add(hDifferentJoe);
            hCol->add(hDifferentJoe);

            initPofWriter();
            m_hWriter->writeCollection(-1, hCol);
            initPofReader();
            Collection::View vColR = cast<Collection::View> (
                    m_hReader->readCollection(-1));
            TS_ASSERT(vColR->size() == hCol->size());

            PortablePerson::View vPerson = NULL;
            int32_t i = 0;
            for (Iterator::Handle hIter = vColR->iterator(); hIter->hasNext();)
                {
                PortablePerson::View vPersonNext = cast<PortablePerson::View> (
                        hIter->next());
                if (NULL == vPerson)
                    {
                    vPerson = vPersonNext;
                    i++;
                    }
                else
                    {
                    TS_ASSERT(vPerson->equals(vPersonNext));
                    if (i == 1 || i == 3)
                        {
                        TS_ASSERT(vPerson == vPersonNext);
                        }
                    else
                        {
                        vPerson = vPersonNext;
                        }
                    i++;
                    }
                }
            }

        /**
         * Test the PofBufferWriter::writeObjectArray() method with uniform
         * object array.
         */
        void testReferencesInUniformArray()
            {
            PortablePersonReference::Handle hIvan     =
                    PortablePersonReference::create(String::create("Ivan"), 42);
            PortablePersonReference::Handle hGoran    =
                    PortablePersonReference::create(String::create("Goran"), 45);
            ObjectArray::Handle             hChildren = ObjectArray::create(2);
            hChildren[0] = PortablePerson::create(String::create("Tom"), 17);
            hChildren[1] = PortablePerson::create(String::create("Ellen"), 20);
            hGoran->setChildren(hChildren);

            ObjectArray::Handle hIvanSib  = ObjectArray::create(1);
            ObjectArray::Handle hGoranSib = ObjectArray::create(1);
            hIvanSib[0] = hGoran;
            hGoranSib[0] = hIvan;
            hIvan->setSiblings(hIvanSib);
            hGoran->setSiblings(hGoranSib);

            Map::Handle          hMapPerson = HashMap::create();
            String::View         hLastName  = String::create("Smith");
            CompositeKey::Handle hKey1      = CompositeKey::create(hLastName,
                    String::create("ivan")),
                                 hKey2      = CompositeKey::create(
                    hLastName, String::create("goran"));
            hMapPerson->put(hKey1, hIvan);
            hMapPerson->put(hKey2, hGoran);

            initPofWriter();
            if (m_hCtx->isReferenceEnabled())
                {
                m_hWriter->enableReference();
                }
            m_hWriter->writeMap(-1, hMapPerson);

            initPofReader();

            Map::View vMapResult = m_hReader->readMap(-1);
            TS_ASSERT(2 == vMapResult->size());

            PortablePersonReference::View vIvanR     = cast<
                    PortablePersonReference::View> (vMapResult->get(hKey1));
            PortablePersonReference::View vGoranR    = cast<
                    PortablePersonReference::View> (vMapResult->get(hKey2));
            ObjectArray::View             hIvranRSib = vIvanR->getSiblings();
            Collection::View              vKeySet    = vMapResult->keySet();
            Iterator::Handle              hIter      = vKeySet->iterator();
            CompositeKey::View            vKey1R     =
                    cast<CompositeKey::View> (hIter->next());
            CompositeKey::View            vKey2R     =
                    cast<CompositeKey::View> (hIter->next());

            TS_ASSERT(vKey1R->getPrimaryKey() != vKey2R->getPrimaryKey());
            TS_ASSERT(hIvranRSib[0] == vGoranR);
            TS_ASSERT(hGoran->getName()->equals(vGoranR->getName()));
            TS_ASSERT(NULL == vIvanR->getChildren());
            }

        void testReferencesInUniformMap()
            {
            SystemClassLoader::Handle       hLoader    =
                    SystemClassLoader::getInstance();
            Map::Handle                     hMapPerson = HashMap::create();
            String::View                    vLastName  = String::create("Smith");
            CompositeKey::Handle            hKey1      = CompositeKey::create(
                    vLastName, String::create("ivan")),
                                            hKey2      = CompositeKey::create(
                    vLastName, String::create("goran"));
            PortablePersonReference::Handle hIvan      =
                    PortablePersonReference::create(String::create("Ivan"), 42);
            hIvan->setChildren(NULL);
            hMapPerson->put(hKey1, hIvan);
            hMapPerson->put(hKey2, hIvan);

            initPofWriter();
            m_hWriter->writeMap(-1, hMapPerson, hLoader->loadByType(
                    typeid(CompositeKey)), hLoader->loadByType(
                    typeid(PortablePersonReference)));

            initPofReader();
            Map::View        vMapPersonR = m_hReader->readMap(-1);
            Collection::View vColVal     = hMapPerson->values();
            Collection::View vColValR    = vMapPersonR->values();
            Collection::View vColKey     = hMapPerson->keySet();
            Collection::View vColKeyR    = vMapPersonR->keySet();

            // compare mapPerson with result
            TS_ASSERT(vColVal->containsAll(vColValR));
            TS_ASSERT(vColKey->containsAll(vColKeyR));
            TS_ASSERT(vColVal->size() == vColValR->size());
            TS_ASSERT(vColKey->size() == vColKeyR->size());
            TS_ASSERT(vMapPersonR->get(hKey1) == vMapPersonR->get(hKey2));
            }

        void testReferencesInArray()
            {
            SystemClassLoader::Handle       hLoader    =
                    SystemClassLoader::getInstance();
            PortablePersonReference::Handle hIvan      =
                    PortablePersonReference::create(String::create("Ivan"), 42);
            PortablePersonReference::Handle hGoran     =
                    PortablePersonReference::create(String::create("Goran"), 45);
            PortablePersonReference::Handle hJack      =
                    PortablePersonReference::create(String::create("Jack"), 21);
            PortablePersonReference::Handle hJim       =
                    PortablePersonReference::create(String::create("Jim"), 18);
            ObjectArray::Handle             haSiblings = ObjectArray::create(2);
            ObjectArray::Handle             haChildren = ObjectArray::create(2);
            haSiblings[0] = hJack;
            haSiblings[1] = hJim;

            hGoran->setChildren(haChildren);
            haChildren[0] = PortablePerson::create(String::create("Tom"), 17);
            haChildren[1] = PortablePerson::create(String::create("Ellen"), 20);
            hIvan->setSiblings(haSiblings);
            hGoran->setSiblings(haSiblings);
            TS_ASSERT(hIvan->getSiblings() == hGoran->getSiblings());

            Collection::Handle hCol1 = ArrayList::create();
            hCol1->add(hIvan);
            hCol1->add(hGoran);

            initPofWriter();
            m_hWriter->writeCollection(0, hCol1);
            m_hWriter->writeCollection(0, hCol1, hLoader->loadByType(
                    typeid(PortablePersonReference)));

            initPofReader();
            Collection::View vResult  = cast<Collection::View> (
                    m_hReader->readCollection(-1));
            Collection::View vResult2 = cast<Collection::View> (
                    m_hReader->readCollection(-1));
            TS_ASSERT(2 == vResult->size());

            PortablePersonReference::View vIvanR  = cast<
                    PortablePersonReference::View> (vResult->toArray()[0]);
            PortablePersonReference::View vGoranR = cast<
                    PortablePersonReference::View> (vResult->toArray()[1]);

            TS_ASSERT(vIvanR->getSiblings() != vGoranR->getSiblings());
            TS_ASSERT(vIvanR->getSiblings()[0] == vGoranR->getSiblings()[0]);
            TS_ASSERT(vIvanR->getSiblings()[1] == vGoranR->getSiblings()[1]);
            TS_ASSERT(NULL == vIvanR->getChildren());

            PortablePersonReference::View vIvanR2  = cast<
                    PortablePersonReference::View> (vResult2->toArray()[0]);
            PortablePersonReference::View vGoranR2 = cast<
                    PortablePersonReference::View> (vResult2->toArray()[1]);

            TS_ASSERT(vIvanR2->getSiblings() != vGoranR2->getSiblings());
            TS_ASSERT(vIvanR2->getSiblings()[0] == vGoranR2->getSiblings()[0]);
            TS_ASSERT(vIvanR2->getSiblings()[1] == vGoranR2->getSiblings()[1]);
            TS_ASSERT(NULL == vIvanR2->getChildren());
            }

        void testEvolvableObjectSerialization()
            {
            SystemClassLoader::Handle hLoader =
                    SystemClassLoader::getInstance();
            SimplePofContext::Handle  hCtxV2  = SimplePofContext::create();
            hCtxV2->registerUserType(1001,
                    hLoader->loadByType(typeid(Address)),
                    PortableObjectSerializer::create(1001));
            hCtxV2->registerUserType(1002, hLoader->loadByType(
                    typeid(PortablePerson)), PortableObjectSerializer::create(
                    1002));
            hCtxV2->registerUserType(1003, hLoader->loadByType(
                    typeid(EvolvablePortablePerson)),
                    PortableObjectSerializer::create(1003));
            hCtxV2->registerUserType(1004, hLoader->loadByType(
                    typeid(EvolvablePortablePerson2)),
                    PortableObjectSerializer::create(1004));
            hCtxV2->registerUserType(1005, hLoader->loadByType(
                    typeid(PortablePersonReference)),
                    PortableObjectSerializer::create(1005));
            hCtxV2->registerUserType(1010, hLoader->loadByType(
                    typeid(CompositeKey)), PortableObjectSerializer::create(
                    1010));
            hCtxV2->setReferenceEnabled(true);

            EvolvablePortablePerson2::Handle hPerson12 =
                    EvolvablePortablePerson2::create(String::create(
                            "Aleksandar Seovic"), 42);
            EvolvablePortablePerson2::Handle hPerson22 =
                    EvolvablePortablePerson2::create(String::create(
                            "Ana Maria Seovic"), 20);
            EvolvablePortablePerson2::Handle hPerson32 =
                    EvolvablePortablePerson2::create(String::create(
                            "Art Seovic"), 17);
            ObjectArray::Handle              hChildren = ObjectArray::create(2);
            Address::Handle                  hAddr     = Address::create(
                    String::create("208 Myrtle Ridge Rd"), String::create("Lutz"),
                    String::create("FL"), String::create("33549"));
            Address::Handle                  hAddrPOB  = Address::create(
                    String::create("128 Asbury Ave, #401"), String::create("Evanston"),
                    String::create("IL"), String::create("60202"));
            hPerson12->setAddress(hAddr);
            hPerson22->setAddress(hAddr);
            hPerson12->setNationality(String::create("Serbian"));
            hPerson22->setNationality(String::create("Serbian"));
            hPerson12->setPOB(Address::create(String::null_string,
                    String::create("Belgrade"), String::create("Serbia"),
                    String::create("11000")));
            hPerson22->setPOB(hAddrPOB);
            hPerson32->setPOB(hAddrPOB);
            hChildren[0] = hPerson22;
            hChildren[1] = hPerson32;
            hPerson12->setChildren(hChildren);

            Array<octet_t>::Handle habV2 = Array<octet_t>::create(2048);
            hCtxV2->serialize(
                    OctetArrayWriteBuffer::create(habV2)->getBufferOutput(),
                    hPerson12);

            EvolvablePortablePerson::Handle hPerson11 = cast<
                    EvolvablePortablePerson::Handle> (m_hCtx->deserialize(
                    OctetArrayReadBuffer::create(habV2)->getBufferInput()));
            EvolvablePortablePerson::Handle hPerson21 =
                    EvolvablePortablePerson::create(String::create(
                            "Marija Seovic"), 20);
            hPerson21->setAddress(hPerson11->getAddress());
            hPerson11->setSpouse(hPerson21);

            Array<octet_t>::Handle habV1 = Array<octet_t>::create(2048);
            m_hCtx->serialize(
                    OctetArrayWriteBuffer::create(habV1)->getBufferOutput(),
                    hPerson11);

            EvolvablePortablePerson2::View vPerson = cast<
                    EvolvablePortablePerson2::View> (hCtxV2->deserialize(
                    OctetArrayReadBuffer::create(habV1)->getBufferInput()));

            TS_ASSERT(hPerson12->getName()->equals(vPerson->getName()));
            TS_ASSERT(hPerson12->getNationality()->equals(vPerson->getNationality()));
            TS_ASSERT(hPerson12->getAge() == vPerson->getAge());
            TS_ASSERT(hPerson11->getSpouse()->getName()->equals(vPerson->getSpouse()->getName()));
            TS_ASSERT(hPerson12->getAddress()->equals(vPerson->getAddress()));
            TS_ASSERT(hPerson12->getPOB()->equals(vPerson->getPOB()));

            //reference is not supported for EvolvablePortableObject
            TS_ASSERT(vPerson->getAddress() != cast<PortablePerson::View>(vPerson->getChildren()[0])->getAddress());
            TS_ASSERT(vPerson->getAddress() != vPerson->getSpouse()->getAddress());

            // using PofBufferWriter and PofBufferReader directly
            initPofWriter();
            m_hWriter->writeObject(0, hPerson12);

            initPofReader();
            hPerson11 = cast<EvolvablePortablePerson::Handle> (
                    m_hReader->readObject(-1));
            hPerson11->setSpouse(hPerson21);

            initPofWriter();
            m_hWriter->writeObject(0, hPerson11);

            initPofReader();
            hPerson11 = cast<EvolvablePortablePerson2::Handle> (
                    m_hReader->readObject(-1));

            TS_ASSERT(hPerson12->getName()->equals(vPerson->getName()));
            TS_ASSERT(hPerson12->getNationality()->equals(vPerson->getNationality()));
            TS_ASSERT(hPerson12->getAge() == vPerson->getAge());
            TS_ASSERT(hPerson11->getSpouse()->getName()->equals(vPerson->getSpouse()->getName()));
            TS_ASSERT(hPerson12->getAddress()->equals(vPerson->getAddress()));
            TS_ASSERT(hPerson12->getPOB()->equals(vPerson->getPOB()));
            TS_ASSERT(vPerson->getAddress() != cast<PortablePerson::View>(vPerson->getChildren()[0])->getAddress());
            TS_ASSERT(vPerson->getAddress() != vPerson->getSpouse()->getAddress());
            }

        void testCircularReferences()
            {
            PortablePersonReference::Handle hIvan       =
                    PortablePersonReference::create(String::create("Ivan"), 42);
            PortablePersonReference::Handle hGoran      =
                    PortablePersonReference::create(String::create("Goran"), 45);
            ObjectArray::Handle             haChildren  = ObjectArray::create(1);
            ObjectArray::Handle             haChildrenG = ObjectArray::create(2);
            hIvan->setChildren(haChildren);
            haChildren[0] = PortablePerson::create(String::create("Mary Jane"),
                    20);
            hIvan->setSpouse(PortablePersonReference::create(String::create(
                    "Eda"), 40));

            hGoran->setChildren(haChildrenG);
            haChildrenG[0] = PortablePerson::create(String::create("Tom"), 19);
            haChildrenG[1]
                    = PortablePerson::create(String::create("Ellen"), 17);
            hGoran->setSpouse(PortablePersonReference::create(String::create(
                    "Tiffany"), 40));
            hGoran->setFriend(hIvan);
            hIvan->setFriend(hGoran);

            initPofWriter();
            m_hWriter->writeObject(0, hIvan);

            initPofReader();
            PortablePersonReference::View vIvanR = cast<
                    PortablePersonReference::View> (m_hReader->readObject(-1));
            TS_ASSERT(vIvanR->getName()->equals(hIvan->getName()));
            TS_ASSERT(vIvanR->getChildren()->length == 1);
            TS_ASSERT(vIvanR->getFriend()->equals(hGoran));
            TS_ASSERT(vIvanR == vIvanR->getFriend()->getFriend());
            }

        /**
         * Test Nested Type.
         */
        void testNestedType()
            {
            SystemClassLoader::Handle hLoader =
                    SystemClassLoader::getInstance();
            hLoader->registerClass(coherence::lang::TypedClass<
                    NestedTypeWithReference>::create());

            initPofWriter();
            m_hCtx->registerUserType(1011, hLoader->loadByType(
                    typeid(NestedTypeWithReference)),
                    PortableObjectSerializer::create(1011));

            NestedTypeWithReference::Handle hTv =
                    NestedTypeWithReference::create();
            m_hWriter->writeObject(0, hTv);

            initPofReader();
            NestedTypeWithReference::View vResult = cast<
                    NestedTypeWithReference::View> (m_hReader->readObject(-1));
            }
    };
