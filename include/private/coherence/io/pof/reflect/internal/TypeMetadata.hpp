/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPE_METADATA_HPP
#define COH_TYPE_METADATA_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/reflect/Codec.hpp"

#include "coherence/util/Iterator.hpp"
#include "coherence/util/List.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

using coherence::io::pof::reflect::Codec;

using coherence::util::List;
using coherence::util::Iterator;

/**
* TypeMetadata represents the definition of a type including uniqueness
* (TypeKey) and all structural properties. This definition is used
* to uniformly define types and their internal structures. Uniformity in
* this context is in relation to the supported languages.
* <p>
* This interface defines the contract required by users of TypeMetadata. This
* includes the ability to have a predictable order for both getter and setter
* methods, the ability to retrieve a method, and to create a new instance of
* a type this metadata describes.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT TypeMetadata
    : public interface_spec<TypeMetadata>
    {
    // ----- inner interface: TypeKey ---------------------------------------

    public:
        /**
        * A type key embodies contributors to the uniqueness representing a
        * TypeMetadata instance. This is the sum of typeId, versionId and a
        * hash.
        *
        * @author hr 2011.06.29
        *
        * @since  3.7.1
        */
        class COH_EXPORT TypeKey
            : public interface_spec<TypeKey>
            {

            public:
                /**
                * An integer identifying a unique pof user type providing the
                * ability to distinguish between types using a compact form.
                *
                * @return pof user type identifier
                */
                virtual int32_t getTypeId() const = 0;

                /**
                * The version specified by the serializer when this object
                * was serialized.
                *
                * @return integer representing the version of this POF type
                */
                virtual int32_t getVersionId() const = 0;

                /**
                * A unique hash representing the TypeMetadata structure.
                *
                * @return hash of TypeMetadata
                */
                virtual int32_t getHash() const = 0;
            };

        // ----- inner interface: AttributeMetadata -------------------------

        /**
        * AttributeMetadata represents all appropriate information relating
        * to an attribute within a type. This contract has similar forms in
        * all supported languages providing a language agnostic mechanism to
        * describe elements within a structure and an invocation mechanism
        * for setting or retrieving the value for an attribute.
        *
        * @author hr 2011.06.29
        *
        * @since  3.7.1
        */
        class COH_EXPORT AttributeMetadata
            : public interface_spec<AttributeMetadata>
            {
            public:
                /**
                * Name of the attribute this metadata describes.
                *
                * @return attribute name
                */
                virtual String::View getName() const = 0;

                /**
                * Returns the versionId assigned to this attributes metadata
                * instance. This versionId is not required however is used as
                * an indicator to determine the version this attribute was
                * introduced in.
                *
                * @return integer representing the version of this attribute
                *         metadata
                */
                virtual int32_t getVersionId() const = 0;

                /**
                * The index used to order the attributes when iterated by the
                * containing TypeMetadata class.
                *
                * @return index to identify this attribute's position in a
                *         sequence
                */
                virtual int32_t getIndex() const = 0;

                /**
                * The codec assigned to this attribute which will perform
                * typ esafe (de)serialization.
                *
                * @return the Codec used to (de)serialize this attribute
                */
                virtual Codec::View getCodec() const = 0;

                /**
                * Returns the value of the attribute contained within the
                * given object.
                *
                * @param ovContainer  the containing object
                *
                * @return the attribute value stored on the object passed in
                */
                virtual Object::Holder get(Object::View ovContainer) const = 0;

                /**
                * Sets the value of this attribute within the given
                * object.
                *
                * @param ohContainer  the containing object
                * @param ohValue      the value to set this attribute to
                */
                virtual void set(Object::Holder ohContainer, Object::Holder ohValue) const = 0;
            };

    // ----- TypeMetadata interface -----------------------------------------

    public:
        /**
        * Return a unique key for this TypeMetaData.
        *
        * @return TypeKey uniquely identifying an instance of TypeMetadata
        */
        virtual TypeKey::View getKey() const = 0;

        /**
        * Create a new instance of the object represented by this type.
        *
        * @return new object instance represented by this metadata
        */
        virtual Object::Holder newInstance() const = 0;

        /**
        * Provides a predictable Iterator over AttributeMetadata
        * for the attributes of the type represented by this TypeMetadata.
        *
        * @return Iterator of AttributeMetadata
        */
        virtual Iterator::Handle getAttributes() const = 0;

        /**
        * Provides a AttributeMetadata encapsulating the
        * method requested.
        *
        * @param vsName  name of the attribute
        *
        * @return AttributeMetadata representing the annotated method
        */
        virtual AttributeMetadata::View getAttribute(String::View vsName) const  = 0;
    };
    
COH_CLOSE_NAMESPACE5

#endif // COH_TYPE_METADATA_HPP
