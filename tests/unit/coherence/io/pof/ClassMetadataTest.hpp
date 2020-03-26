/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "PofAnnotationObjects.hpp"

#include "coherence/io/pof/reflect/Codecs.hpp"

#include "private/coherence/io/pof/reflect/internal/ClassAttributeBuilder.hpp"
#include "private/coherence/io/pof/reflect/internal/ClassMetadataBuilder.hpp"
#include "private/coherence/io/pof/reflect/internal/InvocationStrategies.hpp"

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using annotations_test::Poffed;

using coherence::io::pof::reflect::Codecs;

using coherence::io::pof::reflect::internal::ClassAttributeBuilder;
using coherence::io::pof::reflect::internal::ClassMetadata;
using coherence::io::pof::reflect::internal::ClassMetadataBuilder;
using coherence::io::pof::reflect::internal::InvocationStrategies;

/**
* Test suite for ClassMetadata building.
*/
class ClassMetadataTest : public CxxTest::TestSuite
    {
    public:
        /**
         * Test builder.
         */
        void testBuilder()
            {
            ClassLoader::View             vLoader  = SystemClassLoader::getInstance();
            Class::View                   vcPoffed = vLoader->loadByType(typeid(Poffed));
            ClassMetadataBuilder::Handle  cmdb     = ClassMetadataBuilder::create();
            ClassAttributeBuilder::Handle cab      = ClassAttributeBuilder::create();

            TypeMetadata::View tmd = cmdb->setClass ( vcPoffed )
                ->setHash(321)
                ->setTypeId(1234)
                ->setVersionId(1)
                ->addAttribute( cmdb->newAttribute()
                     ->setIndex(0)
                     ->setName("normal")
//                     ->setFieldName("m_normal")
                     ->setVersionId(0)->build() )->build();

            TS_ASSERT(tmd->getKey()->getTypeId()==1234);
            TS_ASSERT(tmd->getKey()->getHash()!=0);
            TS_ASSERT(tmd->getKey()->getVersionId()==1);
            TS_ASSERT(tmd->getAttribute("normal")!=NULL);
            TS_ASSERT(tmd->getAttribute("normal")->getName()->equals("normal"));
//             TS_ASSERT(((ClassMetadata.ClassAttribute)tmd.getAttribute("normal")).getFieldName(),"m_normal");
            TS_ASSERT(tmd->getAttribute("normal")->getIndex()==0);
            TS_ASSERT(tmd->getAttribute("normal")->getVersionId()==0);
            }

        void testClassMetadata()
            {
            ClassLoader::View             vLoader  = SystemClassLoader::getInstance();
            Class::View                   vcPoffed = vLoader->loadByType(typeid(Poffed));
            ClassMetadataBuilder::Handle  cmdb     = ClassMetadataBuilder::create();
            ClassAttributeBuilder::Handle cab      = ClassAttributeBuilder::create();

            TypeMetadata::View tmd = cmdb->setClass ( vcPoffed )
                ->setHash(321)
                ->setTypeId(1234)
                ->setVersionId(1)
                ->addAttribute( cmdb->newAttribute()
                     ->setIndex(0)
                     ->setName("firstName")
                     ->setCodec( Codecs::getDefault() )
//                   ->setInvocationStrategy( new InvocationStrategies.MethodInvocationStrategy<ClassMetadataDescribable,String>( method, ClassMetadataDescribable.class ) )
                     ->setVersionId(0)->build() )->build();

            TypeMetadata::View tmd2 = cmdb->setClass ( vcPoffed )
                ->setHash(321)
                ->setTypeId(1234)
                ->setVersionId(1)
                ->addAttribute( cmdb->newAttribute()
                     ->setIndex(0)
                     ->setName("firstName")
                     ->setCodec( Codecs::getDefault() )
//                   ->setInvocationStrategy( new InvocationStrategies.MethodInvocationStrategy<ClassMetadataDescribable,String>( method, ClassMetadataDescribable.class ) )
                     ->setVersionId(0)->build() )->build();

            Method::View vMethod = vcPoffed->getDeclaredMethod("getFirstName");

            TypeMetadata::View tmd3 = cmdb->setClass ( vcPoffed )
                ->setHash(321)
                ->setTypeId(1235)
                ->setVersionId(1)
                ->addAttribute( cmdb->newAttribute()
                     ->setIndex(1)
                     ->setName("firstName")
                     ->setCodec( Codecs::getDefault() )
                     ->setInvocationStrategy(InvocationStrategies::MethodInvocationStrategy::create(vMethod))
                     ->setVersionId(0)->build() )->build();

            TS_ASSERT_EQUALS (1234, tmd->getKey()->getTypeId());
            TS_ASSERT_DIFFERS(0, tmd->getKey()->getHash());
            TS_ASSERT_EQUALS (1, tmd->getKey()->getVersionId());
            TS_ASSERT        (tmd->getAttribute("firstName") != NULL);
            TS_ASSERT        (tmd->getAttribute("firstName")->getName()->equals("firstName"));
            // TS_ASSERT(((ClassMetadata.ClassAttribute)tmd.getAttribute("normal")).getFieldName(),"m_normal");
            TS_ASSERT_EQUALS (0, tmd->getAttribute("firstName")->getIndex());
            TS_ASSERT_EQUALS (0, tmd->getAttribute("firstName")->getVersionId());
            TS_ASSERT        (tmd->getKey()->equals(tmd2->getKey()));
            TS_ASSERT_EQUALS (tmd->getKey()->hashCode(), tmd2->getKey()->hashCode());
            TS_ASSERT        (!tmd->getKey()->equals(tmd3->getKey()));
            TS_ASSERT_DIFFERS(tmd->getKey()->hashCode(), tmd3->getKey()->hashCode());
            TS_ASSERT        (tmd->getAttribute("firstName")->equals( tmd2->getAttribute("firstName")));
            TS_ASSERT_EQUALS (tmd->getAttribute("firstName")->hashCode(), tmd2->getAttribute("firstName")->hashCode());
            TS_ASSERT        (!tmd->getAttribute("firstName")->equals(tmd3->getAttribute("firstName")));
            TS_ASSERT_DIFFERS(tmd->getAttribute("firstName")->hashCode(), tmd3->getAttribute("firstName")->hashCode());

            Poffed::Handle vPerson = Poffed::create("augusta");
            TS_ASSERT(cast<String::View>(tmd3->getAttribute("firstName")->get(vPerson))->equals("augusta"));
            tmd3->getAttribute("firstName")->set(vPerson, String::create("ada"));
            TS_ASSERT(cast<String::View>(tmd3->getAttribute("firstName")->get(vPerson))->equals("ada"));
            // tmd3->getAttribute("firstName")->set(vPerson, NULL);
            // TS_ASSERT(NULL == tmd3->getAttribute("firstName")->get(vPerson));
            }

        void testClassAttributeSort()
            {
            ClassMetadataBuilder::Handle  cmdb         = ClassMetadataBuilder::create();
            ClassAttributeBuilder::Handle cab          = ClassAttributeBuilder::create();
            ObjectArray::Handle           haAttributes = ObjectArray::create(10);

            haAttributes[0] = cab->setVersionId(0)->setIndex( 0)->setName("c")->build();
            haAttributes[1] = cab->setVersionId(0)->setIndex( 1)->setName("a")->build();
            haAttributes[2] = cab->setVersionId(0)->setIndex( 2)->setName("b")->build();
            haAttributes[3] = cab->setVersionId(1)->setIndex( 3)->setName("e")->build();
            haAttributes[4] = cab->setVersionId(1)->setIndex(-1)->setName("d")->build();
            haAttributes[5] = cab->setVersionId(1)->setIndex(-1)->setName("f")->build();

            TypeMetadata::View tmd = cmdb->addAttribute( cast<TypeMetadata::AttributeMetadata::Handle>(haAttributes[3]) )
                                         ->addAttribute( cast<TypeMetadata::AttributeMetadata::Handle>(haAttributes[1]) )
                                         ->addAttribute( cast<TypeMetadata::AttributeMetadata::Handle>(haAttributes[5]) )
                                         ->addAttribute( cast<TypeMetadata::AttributeMetadata::Handle>(haAttributes[0]) )
                                         ->addAttribute( cast<TypeMetadata::AttributeMetadata::Handle>(haAttributes[4]) )
                                         ->addAttribute( cast<TypeMetadata::AttributeMetadata::Handle>(haAttributes[2]) )->build();

            TS_ASSERT(tmd->getAttribute("c")->getIndex()==0);
            TS_ASSERT(tmd->getAttribute("a")->getIndex()==1);
            TS_ASSERT(tmd->getAttribute("b")->getIndex()==2);
            TS_ASSERT(tmd->getAttribute("e")->getIndex()==3);
            TS_ASSERT(tmd->getAttribute("d")->getIndex()==4);
            TS_ASSERT(tmd->getAttribute("f")->getIndex()==5);
            }
    };
