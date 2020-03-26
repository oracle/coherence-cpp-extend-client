/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_METADATA_BUILDER_HPP
#define COH_CLASS_METADATA_BUILDER_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/pof/reflect/internal/ClassAttributeBuilder.hpp"
#include "private/coherence/io/pof/reflect/internal/ClassMetadata.hpp"
#include "private/coherence/io/pof/reflect/internal/Visitor.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
* Class ClassMetadataBuilder provides a simple mechanism to
* instantiate and inject state into a ClassMetadata instance.
* Parsers that read a source will use this builder to derive a
* ClassMetadata destination.
*
* The general usage of this class is to perform multiple chained set calls
* with a final build call which will realize a ClassMetadata instance.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT ClassMetadataBuilder
    : public class_spec<ClassMetadataBuilder,
               extends<Object>,
               implements<Visitor::Recipient> >
    {
    friend class factory<ClassMetadataBuilder>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a ClassMetadataBuilder instance.
        */
        ClassMetadataBuilder();

    // ----- ClassMetadataBuilder methods -----------------------------------

    public:
        /**
        * Specify the class type this ClassMetadata is assigned to.
        *
        * @param vClz  Class that the resulting TypeMetadata instance
        *              describes
        *
        * @return a reference to this for chained set calls
        *
        * @see ClassMetadata::setClass
        */
        virtual ClassMetadataBuilder::Handle setClass(Class::View vClz);

        /**
        * Add an AttributeMetadata instance that is a child of
        * the ClassMetadata instance.
        *
        * @param hAttribute  AttributeMetadata implementation to add to
        *                    the enclosing TypeMetadata instance
        *
        * @return a reference to this for chained set calls
        *
        * @see ClassMetadata::addAttribute
        */
        virtual ClassMetadataBuilder::Handle addAttribute(TypeMetadata::AttributeMetadata::Handle hAttribute);

        /**
        * Creates a new attribute builder for populating an
        * AttributeMetadata instance.
        *
        * @return a ClassAttributeBuilder that builds an attribute
        */
        virtual ClassAttributeBuilder::Handle newAttribute() const;

        /**
        * Based on the state that the builder has been informed of create and
        * return a ClassMetadata instance.
        *
        * @return the built ClassMetadata instance
        */
        virtual ClassMetadata::Holder build();

    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Returns a const TypeMetadata in its current form, i.e. prior to
        * ::build().
        *
        * @return the TypeMetadata instance being enriched
        */
        virtual TypeMetadata::View getTypeMetadata() const;

        /**
        * Specify the unique type id for the TypeKey.
        *
        * @see TypeKey::getTypeId()
        *
        * @param nTypeId  type id used in uniquely identifying a
        *                 TypeMetadata instance
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassMetadataBuilder::Handle setTypeId(int32_t nTypeId);

        /**
        * Specify the version for this TypeMetadata instance.
        *
        * @see TypeMetadata::TypeKey::getVersionId()
        *
        * @param nVersionId  the version of this TypeMetadata instance
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassMetadataBuilder::Handle setVersionId(int32_t nVersionId);

        /**
        * Specify the hash for this TypeMetadata instance.
        *
        * @see TypeMetadata::TypeKey::getHash()
        *
        * @param nHash  a hash value of the TypeMetadata instance
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassMetadataBuilder::Handle setHash(int32_t nHash);

    // ----- Visitor::Recipient interface -----------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void accept(Visitor::View vVisitor, Class::View vClz);

    // ----- data members ---------------------------------------------------

    private:
        /**
        * ClassMetadata that is built across the duration of
        * ClassMetadataBuilder calls until it is returned via the
        * ClassMetadataBuilder::build method.
        */
        MemberHandle<ClassMetadata>           m_hCmd;

        /**
        * ClassKey that is built across the duration of
        * ClassMetadataBuilder calls until it is returned via the
        * ClassMetadataBuilder::build() method.
        */
        MemberHandle<ClassMetadata::ClassKey> m_hKey;
    };

COH_CLOSE_NAMESPACE5

#endif /* COH_CLASS_METADATA_BUILDER_HPP */
