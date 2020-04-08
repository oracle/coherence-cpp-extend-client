/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofConstants.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/ArrayList.hpp"
#include "coherence/util/Collection.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;
using namespace std;
using namespace common::test;

using coherence::io::Serializer;
using coherence::io::pof::PofBufferReader;
using coherence::io::pof::PofConstants;
using coherence::io::pof::PortableObject;
using coherence::io::pof::SystemPofContext;
using coherence::util::Collection;

COH_OPEN_NAMESPACE_ANON(PofUniformCollectionTest)

// ----- local class: MapTestClass ----------------------

class MapTestClass
    : public class_spec<MapTestClass,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<MapTestClass>;

    // ----- constructors ---------------------------------------

    protected:
        MapTestClass()
            : m_vMap(self(), NULL),
              m_fUniformValue(false)
            {
            }

        /**
         * Create a new MapTestClass.
         *
         * @param vMap  the map to use for the test
         * @param fUniformValue flag to indicate if it is a uniform value
         */
        MapTestClass(Map::View vMap, bool fUniformValue)
            : m_vMap(self(), vMap),
              m_fUniformValue(fUniformValue)
            {
            }

    // ----- accessor methods -----------------------------------

    public:
        void setMap(Map::View vMap)
            {
            m_vMap = vMap;
            }

        Map::View getMap() const
            {
            return m_vMap;
            }

    // ----- PortableObject interface ---------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual void readExternal(PofReader::Handle hIn)
            {
            setMap(hIn->readMap(0));
            }

        /**
         * {@inheritDoc}
         */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            Map::View vMap = m_vMap;
            TS_ASSERT(NULL != vMap && !vMap->isEmpty());
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();
            Map::Entry::View        vEntry  = cast<Map::Entry::View>(vMap->entrySet()->iterator()->next());

            if (m_fUniformValue)
                {
                hOut->writeMap(0, vMap,
                        vLoader->loadByName(Class::getClassName(vEntry->getKey())),
                        vLoader->loadByName(Class::getClassName(vEntry->getValue())));
                }
            else
                {
                hOut->writeMap(0, vMap,
                        vLoader->loadByName(Class::getClassName(vEntry->getKey())));
                }
            }

    // ----- Object interface -----------------------------------

    public:
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("MapTestClass[map=" << m_vMap << ", uniformValue=" << "]");
            }

        virtual size32_t hashCode() const
            {
            const int prime  = 31;
            int       result = 1;
            result = prime * result + ((NULL == m_vMap) ? 0 : m_vMap->hashCode());
            return result;
            }

        virtual bool equals(Object::View vThat) const
            {
            if (this == vThat)
                {
                return true;
                }
            if (NULL == vThat)
                {
                return false;
                }
            Map::View vThatMap = cast<MapTestClass::View>(vThat)->m_vMap;
            return m_vMap->equals(vThatMap);
            }

    // ----- data members ---------------------------------------

    private:
        MemberView<Map> m_vMap;
        bool            m_fUniformValue;
    };

// ----- local class: CollectionTestClass -------------------------------

class CollectionTestClass
    : public class_spec<CollectionTestClass,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<CollectionTestClass>;

    // ----- constructors ---------------------------------------

    protected:
        CollectionTestClass()
            : m_vCollection(self(), NULL)
            {
            }

        /**
         * Create a new CollectionTestClass.
         *
         * @param vCollection  the collection to use for the test
         */
        CollectionTestClass(Collection::View vCollection)
            : m_vCollection(self(), vCollection)
            {
            }

    // ----- accessor methods -----------------------------------

    public:
        void setCollection(Collection::View vCollection)
            {
            m_vCollection = vCollection;
            }

        Collection::View getCollection() const
            {
            return m_vCollection;
            }

    // ----- PortableObject interface ---------------------------

    public:
        /**
         * {@inheritDoc}
         */
        virtual void readExternal(PofReader::Handle hIn)
            {
            setCollection(hIn->readCollection(0));
            }

        /**
         * {@inheritDoc}
         */
        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            Collection::View vColl = getCollection();
            TS_ASSERT(NULL != vColl && vColl->size() > 0);
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();
            hOut->writeCollection(0, vColl, vLoader->loadByName(Class::getClassName(vColl->iterator()->next())));
            }

    // ----- Object interface -----------------------------------

    public:
        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("CollectionTestClass[collection=" << m_vCollection << "]");
            }

        virtual size32_t hashCode() const
            {
            const int prime  = 31;
            int       result = 1;
            result = prime * result + ((NULL == m_vCollection) ? 0 : m_vCollection->hashCode());
            return result;
            }

        virtual bool equals(Object::View vThat) const
            {
            if (this == vThat)
                {
                return true;
                }
            if (NULL == vThat)
                {
                return false;
                }
            Collection::View vThatCollection = cast<CollectionTestClass::View>(vThat)->m_vCollection;
            return m_vCollection->equals(vThatCollection);
            }

    // ----- data members ---------------------------------------

    private:
        MemberView<Collection> m_vCollection;
    };

COH_CLOSE_NAMESPACE_ANON

COH_REGISTER_TYPED_CLASS(MapTestClass);
COH_REGISTER_TYPED_CLASS(CollectionTestClass);

/**
 * A collection of POF unit tests that test the PofConstants::t_identity in
 * uniform collections.
 * @author lh 2013.08.07
 */
class PofUniformCollectionTest: public CxxTest::TestSuite
    {
    public:
        void testIntegerStringMapNone()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(PofConstants::t_identity), String::create("A string"));

            verifyPofSerialization(hMap);
            }

        void testIntegerStringMap()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(PofConstants::t_identity), String::create("A string"));

            verifyPofSerialization(MapTestClass::create(hMap, true));
            }

        void testIntegerStringKeyOnly()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(Integer32::create(PofConstants::t_identity), String::create("A string"));

            verifyPofSerialization(MapTestClass::create(hMap, false));
            }

        void testStringIntegerKeyOnly()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("A string"), Integer32::create(PofConstants::t_identity));

            verifyPofSerialization(MapTestClass::create(hMap, false));
            }

         void testStringIntegerMap()
            {
            Map::Handle hMap = HashMap::create();
            hMap->put(String::create("A string"), Integer32::create(PofConstants::t_identity));

            verifyPofSerialization(MapTestClass::create(hMap, true));
            }

        void testIntegerCollection()
            {
            Collection::Handle hCollection = ArrayList::create();
            hCollection->add(Integer32::create(PofConstants::t_identity));

            verifyPofSerialization(CollectionTestClass::create(hCollection));
            }

    // ----- help methods ---------------------------------------------------

    private:
        void init()
            {
            SystemClassLoader::Handle hLoader =
                    SystemClassLoader::getInstance();
            SimplePofContext::Handle  hPofCtx = SimplePofContext::create();
            hPofCtx->registerUserType(1001,
                    hLoader->loadByType(typeid(MapTestClass)),
                    PortableObjectSerializer::create(1001));
            hPofCtx->registerUserType(1002,
                    hLoader->loadByType(typeid(CollectionTestClass)),
                    PortableObjectSerializer::create(1002));
            m_vSerializer = hPofCtx;
            }

        void verifyPofSerialization(Object::View testObject)
            {
            Serializer::View vSerializer = m_vSerializer;
            if (NULL == vSerializer)
                {
                init();
                vSerializer = m_vSerializer;
                }

            Binary::View vBinary = SerializationHelper::toBinary(testObject, vSerializer);
            Object::View v       = SerializationHelper::fromBinary(vBinary, vSerializer);

            TS_ASSERT(testObject->equals(v));
            }

    // ----- data members ---------------------------------------------------
    private:
        Serializer::View m_vSerializer;
    };
