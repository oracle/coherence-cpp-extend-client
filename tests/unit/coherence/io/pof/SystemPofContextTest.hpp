/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/io/pof/PortableObjectSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/util/UUID.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;
using namespace std;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::io::pof::PortableObjectSerializer;
using coherence::io::pof::SystemPofContext;
using coherence::util::UUID;

COH_OPEN_NAMESPACE_ANON(SystemPofContextTest)

class TestObject
    : public class_spec<TestObject>
    {
    friend class factory<TestObject>;
    };
COH_REGISTER_TYPED_CLASS(TestObject);

class TestPortableObject
    : public class_spec<TestPortableObject,
        extends<Object>,
        implements<PortableObject> >
    {
    friend class factory<TestPortableObject>;

    public:
        void readExternal(PofReader::Handle)
            {
            }

        void writeExternal(PofWriter::Handle) const
            {
            }
    };

COH_REGISTER_PORTABLE_CLASS(7777777, TestPortableObject);

COH_CLOSE_NAMESPACE_ANON

/**
* Test suite for the SystemPofContext class.
*/
class SystemPofContextTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test the getPofSerializer(int32_t nTypeId) method.
        */
        void testGetPofSerializer()
            {
            SystemPofContext::Handle hCtx = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->getPofSerializer(7777777) != NULL);
            try
                {
                hCtx->getPofSerializer(1234567);
                TS_ASSERT(false);
                }
            catch (IllegalArgumentException::View) {}
            }

        /**
        * Test the getUserTypeIdentifier(Object::View v) method.
        */
        void testGetUserTypeIdentifierForObject()
            {
            SystemPofContext::Handle hCtx = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->getUserTypeIdentifier(((Object::View) TestPortableObject::create())) == 7777777);
            try
                {
                hCtx->getUserTypeIdentifier((Object::View) TestObject::create());
                TS_ASSERT(false);
                }
            catch (IllegalArgumentException::View) {}
            }

        /**
        * Test the getUserTypeIdentifier(Class::View vClass) method.
        */
        void testGetUserTypeIdentifierForClass()
            {
            SystemClassLoader::Handle hLoader = SystemClassLoader::getInstance();
            SystemPofContext::Handle  hCtx    = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->getUserTypeIdentifier(hLoader->loadByType(typeid(TestPortableObject))) == 7777777);
            try
                {
                hCtx->getUserTypeIdentifier(hLoader->loadByType(typeid(TestObject)));
                TS_ASSERT(false);
                }
            catch (IllegalArgumentException::View) {}
            }

        /**
        * Test the getUserTypeIdentifier(Class::View vClass) method for a
        * "standard" POF user type.
        */
        void testGetUserTypeIdentifierForStandardClass()
            {
            SystemClassLoader::Handle hLoader = SystemClassLoader::getInstance();
            SystemPofContext::Handle  hCtx    = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->getUserTypeIdentifier(hLoader->loadByType(typeid(Exception))) == 0);
            TS_ASSERT(hCtx->getUserTypeIdentifier(hLoader->loadByType(typeid(UUID))) == 14);
            }

        /**
        * Test the getClassName(int32_t nTypeId)  method.
        */
        void testGetClassName()
            {
            SystemPofContext::Handle hCtx = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->getClassName(7777777)->equals(Class::getClassName(TestPortableObject::create())));
            try
                {
                hCtx->getClassName(1234567);
                TS_ASSERT(false);
                }
            catch (IllegalArgumentException::View) {}
            }

        /**
        * Test the getClass(int32_t nTypeId)  method.
        */
        void testGetClass()
            {
            SystemClassLoader::Handle hLoader = SystemClassLoader::getInstance();
            SystemPofContext::Handle  hCtx    = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->getClass(7777777)->equals(hLoader->loadByType(typeid(TestPortableObject))));
            try
                {
                hCtx->getClass(1234567);
                TS_ASSERT(false);
                }
            catch (IllegalArgumentException::View) {}
            }

        /**
        * Test the isUserType(Object::View v) method.
        */
        void testIsUserTypeForObject()
            {
            SystemPofContext::Handle hCtx = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->isUserType(((Object::View) TestPortableObject::create())));
            TS_ASSERT(!hCtx->isUserType(((Object::View) TestObject::create())));
            }

        /**
        * Test the isUserType(Class::View vClass) method.
        */
        void testIsUserTypeForClass()
            {
            SystemClassLoader::Handle hLoader = SystemClassLoader::getInstance();
            SystemPofContext::Handle  hCtx    = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->isUserType(hLoader->loadByType(typeid(TestPortableObject))));
            TS_ASSERT(!hCtx->isUserType(hLoader->loadByType(typeid(TestObject))));
            }

        /**
        * Test the isUserType(String::View vsClass) method.
        */
        void testIsUserTypeForClassName()
            {
            SystemPofContext::Handle hCtx = SystemPofContext::getInstance();
            TS_ASSERT(hCtx->isUserType(Class::getClassName(TestPortableObject::create())));
            TS_ASSERT(!hCtx->isUserType(Class::getClassName(TestObject::create())));
            }
    };
