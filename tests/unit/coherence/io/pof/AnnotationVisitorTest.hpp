/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "PofAnnotationObjects.hpp"

#include "private/coherence/io/pof/reflect/internal/AnnotationVisitor.hpp"
#include "private/coherence/io/pof/reflect/internal/ClassMetadataBuilder.hpp"
#include "private/coherence/io/pof/reflect/internal/TypeMetadata.hpp"

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using annotations_test::Poffed;
using annotations_test::PoffedImpliedIndicies;
using annotations_test::PoffedClashingIndicies;
using annotations_test::PoffedCustomCodec;
using annotations_test::LinkedListCodec;

using coherence::io::pof::reflect::internal::AnnotationVisitor;
using coherence::io::pof::reflect::internal::ClassMetadataBuilder;
using coherence::io::pof::reflect::internal::TypeMetadata;

/**
* Test suite for AnnotationVisitor implementation.
*/
class AnnotationVisitorTest : public CxxTest::TestSuite
    {
    public:
        /**
         * Test visit.
         */
        void testVisit()
            {
            ClassLoader::View            vLoader  = SystemClassLoader::getInstance();
            Class::View                  vcPoffed = vLoader->loadByType(typeid(Poffed));
            ClassMetadataBuilder::Handle vCmb     = ClassMetadataBuilder::create();
            AnnotationVisitor::Handle    vVisitor = AnnotationVisitor::create();

            vCmb->setTypeId(1001);
            vCmb->accept( vVisitor, vcPoffed );

            TypeMetadata::View tmd = vCmb->build();

            TS_ASSERT(tmd->getKey()->getTypeId()==1001             );
            TS_ASSERT(tmd->getKey()->getVersionId()             ==0);
            TS_ASSERT(tmd->getAttribute("lastName") ->getIndex()==0);
            TS_ASSERT(tmd->getAttribute("age")      ->getIndex()==1);
            TS_ASSERT(tmd->getAttribute("firstName")->getIndex()==2);
            }

        /**
         * Test implied indicies.
         */
        void testImplyIndicies()
            {
            ClassLoader::View            vLoader  = SystemClassLoader::getInstance();
            Class::View                  vcPoffed = vLoader->loadByType(typeid(PoffedImpliedIndicies));
            ClassMetadataBuilder::Handle vCmb     = ClassMetadataBuilder::create();
            AnnotationVisitor::Handle    vVisitor = AnnotationVisitor::create(true);

            vCmb->setTypeId(1002);

            vCmb->accept( vVisitor, vcPoffed );

            TypeMetadata::View tmd = vCmb->build();

            TS_ASSERT(tmd->getKey()->getTypeId()==1002             );
            TS_ASSERT(tmd->getAttribute("lastName") ->getIndex()==2);
            TS_ASSERT(tmd->getAttribute("firstName")->getIndex()==1);
            TS_ASSERT(tmd->getAttribute("age")      ->getIndex()==0);
            }

        /**
         * Test clashing indicies.
         */
        void testClashingIndicies()
            {
            ClassLoader::View            vLoader  = SystemClassLoader::getInstance();
            Class::View                  vcPoffed = vLoader->loadByType(typeid(PoffedClashingIndicies));
            ClassMetadataBuilder::Handle vCmb     = ClassMetadataBuilder::create();
            AnnotationVisitor::Handle    vVisitor = AnnotationVisitor::create(true);

            vCmb->setTypeId(1003);

            vCmb->accept( vVisitor, vcPoffed );

            TypeMetadata::View tmd = vCmb->build();

            TS_ASSERT(tmd->getKey()->getTypeId()==1003             );
            TS_ASSERT(tmd->getKey()->getVersionId()             ==0);
            TS_ASSERT(tmd->getAttribute("lastName") ->getIndex()==3);
            TS_ASSERT(tmd->getAttribute("age")      ->getIndex()==1);
            TS_ASSERT(tmd->getAttribute("firstName")->getIndex()==2);
            TS_ASSERT(tmd->getAttribute("age2")     ->getIndex()==0);
            }

        /**
         * Test custom codec.
         */
        void testCustomCodec()
            {
            ClassLoader::View            vLoader  = SystemClassLoader::getInstance();
            Class::View                  vcPoffed = vLoader->loadByType(typeid(PoffedCustomCodec));
            ClassMetadataBuilder::Handle vCmb     = ClassMetadataBuilder::create();
            AnnotationVisitor::Handle    vVisitor = AnnotationVisitor::create(true);

            vCmb->setTypeId(1004);

            vCmb->accept( vVisitor, vcPoffed );

            TypeMetadata::View tmd = vCmb->build();

            TS_ASSERT(tmd->getKey()->getTypeId()==1004             );
            TS_ASSERT(tmd->getKey()->getVersionId()             ==0);
            TS_ASSERT(tmd->getAttribute("age")      ->getIndex()==0);
            TS_ASSERT(tmd->getAttribute("aliases")  ->getIndex()==1);
            TS_ASSERT(tmd->getAttribute("firstName")->getIndex()==2);
            TS_ASSERT(tmd->getAttribute("lastName") ->getIndex()==3);
            }
    };
