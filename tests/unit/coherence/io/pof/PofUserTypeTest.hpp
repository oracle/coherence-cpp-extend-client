/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/pof/PofHelper.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "coherence/lang/UnsupportedOperationException.hpp"

#include "coherence/util/AbstractList.hpp"

#include "cxxtest/TestSuite.h"

using coherence::io::pof::PofHelper;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::lang::UnsupportedOperationException;

COH_OPEN_NAMESPACE_ANON(PofUserTypeTest)

/**
* Test class used to register a user type id.
*/
class MyPofUserTypeCollection :
     public class_spec<MyPofUserTypeCollection,
         extends<ArrayList>,
         implements<PortableObject> >
    {
    friend class factory<MyPofUserTypeCollection>;

    public:
        // ----- Constructors --------------------------------------

        /*
        * Default constructor.
        */
        MyPofUserTypeCollection()
            : super()
            {
            }


        // ----- PortableObject interface --------------------------

        /**
        * {@inheritDoc}
        */
        void readExternal(PofReader::Handle /* hIn */)
            {
            COH_THROW(UnsupportedOperationException::create("Not implemented"));
            }

        /**
        * {@inheritDoc}
        */
        void writeExternal(PofWriter::Handle /* hOut */) const
            {
            COH_THROW(UnsupportedOperationException::create("Not implemented"));
            }


        // ----- Collection interface ------------------------------

        /**
        * {@inheritDoc}
        */
        virtual bool isEmpty() const
            {
            return super::isEmpty();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool contains(Object::View v) const
            {
            return super::contains(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual bool containsAll(Collection::View vc) const
            {
            return super::containsAll(vc);
            }


        // ----- Object methods ------------------------------------

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const
            {
            return super::equals(v);
            }

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const
            {
            return super::hashCode();
            }

        /**
        * {@inheritDoc}
        */
        virtual bool isImmutable() const
            {
            return super::isImmutable();
            }

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const
            {
            return super::toString();
            }
    };
COH_REGISTER_PORTABLE_CLASS(9999, MyPofUserTypeCollection);

COH_CLOSE_NAMESPACE_ANON

/**
 * Unit tests for getting POF user type ids.
 * Issue reported in COH-9428
 *
 * @author par 2013.4.18
 */
class PofUserTypeTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Test correct user type is returned from context.
        */
        void testIdFromContext()
            {
            SystemPofContext::View  vCtx    = SystemPofContext::getInstance();
            SystemClassLoader::View vLoader = SystemClassLoader::getInstance();

            TS_ASSERT_EQUALS(9999, vCtx->getUserTypeIdentifier(vLoader->loadByType(typeid(MyPofUserTypeCollection))));
            }

        /**
        * Test correct user type is returned from PofHelper.
        */
        void testIdFromPofHelper()
            {
            SystemPofContext::View    vCtx    = SystemPofContext::getInstance();
            SystemClassLoader::Handle vLoader = SystemClassLoader::getInstance();

            TS_ASSERT_EQUALS(9999, PofHelper::getPofTypeId(vLoader->loadByType(typeid(MyPofUserTypeCollection)), vCtx));
            }
     };
