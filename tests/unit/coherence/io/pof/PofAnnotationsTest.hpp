/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "PofAnnotationObjects.hpp"

#include "coherence/io/pof/annotation/Portable.hpp"
#include "coherence/io/pof/annotation/PortableProperty.hpp"

#include "coherence/io/pof/reflect/Codecs.hpp"

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::pof::annotation::Portable;
using coherence::io::pof::annotation::PortableProperty;

using coherence::io::pof::reflect::Codecs;

/**
* Test suite for POF annotations.
*/
class PofAnnotationsTest : public CxxTest::TestSuite
    {
    public:
        /**
         * Test the PofBufferWriter::writeObjectArray() method.
         */
        void testPofAnnotations()
            {
            ClassLoader::View vLoader            = SystemClassLoader::getInstance();
            Class::View       vcPoffed           = vLoader->loadByName("annotations_test::Poffed");
            Class::View       vcPortable         = vLoader->loadByType(typeid(Portable));
            Class::View       vcPortableProperty = vLoader->loadByType(typeid(PortableProperty));
            Class::View       vcObject           = vLoader->loadByType(typeid(Object));
            annotations_test::Poffed::View voPoffed2 = cast<annotations_test::Poffed::View>(vcPoffed->newInstance());

            TS_ASSERT( vcPoffed->isAnnotationPresent( vcPortable ) );

            Portable::View vPortable = cast<Portable::View>(vcPoffed->getAnnotation( vcPortable ));

            // assert method annotations
            ObjectArray::Handle hAParams = ObjectArray::create(1);
            hAParams[0] = vLoader->loadByType(typeid(String));
            ObjectArray::Handle hAAgeParams = ObjectArray::create(1);
            hAAgeParams[0] = vLoader->loadByType(typeid(int32_t));

            Method::View vmGetFirstName = vcPoffed->getMethod("getFirstName");
            Method::View vmFirstName    = vcPoffed->getMethod("setFirstName", hAParams);
            Method::View vmGetLastName  = vcPoffed->getMethod("getLastName" );
            Method::View vmLastName     = vcPoffed->getMethod("setLastName" , hAParams);
            TS_ASSERT( vmFirstName->getParameterTypes()[0]->equals( vLoader->loadByType(typeid(String)) ) );
            TS_ASSERT( vmFirstName->isAnnotationPresent( vcPortableProperty ) );
            TS_ASSERT( vmLastName ->isAnnotationPresent( vcPortableProperty ) );

            PortableProperty::View vPPFirstName = cast<PortableProperty::View>(vmFirstName->getAnnotation( vcPortableProperty ));
            PortableProperty::View vPPLastName  = cast<PortableProperty::View>(vmLastName ->getAnnotation( vcPortableProperty ));

            TS_ASSERT( vPPFirstName->getIndex()    ==2                    );
            TS_ASSERT( vPPFirstName->getCodec()    ==NULL                 );
            TS_ASSERT( vPPLastName ->getIndex()    ==0                    );
            TS_ASSERT( vPPLastName ->getCodec()->equals(vcObject)         );

            String::View vsFirstName = "Harvey";
            String::View vsLastName  = "Raja";
            int32_t      nAge        = 29;
            annotations_test::Poffed::View poffed = annotations_test::Poffed::create(vsFirstName, vsLastName, nAge);

            String::View vsRFirstName = cast<String::View>(vmGetFirstName->invoke(poffed));
            String::View vsRLastName  = cast<String::View>(vmGetLastName->invoke(poffed));

            TS_ASSERT( vsFirstName->equals( vsRFirstName ) );
            TS_ASSERT( vsLastName ->equals( vsRLastName  ) );
            }
    };
