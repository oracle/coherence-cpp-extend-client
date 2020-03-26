/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_METADATA_HPP
#define COH_CLASS_METADATA_HPP

#include "coherence/lang.ns"


#include "coherence/util/Collection.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/io/pof/reflect/internal/InvocationStrategy.hpp"
#include "private/coherence/io/pof/reflect/internal/TypeMetadata.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

using coherence::util::Collection;
using coherence::util::Map;
using coherence::util::Set;

// ----- forward declarations -----------------------------------------------

class ClassAttributeBuilder;
class ClassMetadataBuilder;

/**
* A TypeMetadata implementation that utilizes Coherence RTTI /
* Reflection to perform requests onto the underlying object.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT ClassMetadata
    : public class_spec<ClassMetadata,
               extends<Object>,
               implements<TypeMetadata> >
    {
    friend class factory<ClassMetadata>;
    friend class ClassMetadataBuilder;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Constructs a ClassMetadata instance.
        */
        ClassMetadata();

    // ----- TypeMetadata interface -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypeKey::View getKey() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder newInstance() const;

        /**
        * {@inheritDoc}
        */
        virtual Iterator::Handle getAttributes() const;

        /**
        * {@inheritDoc}
        */
        virtual AttributeMetadata::View getAttribute(String::View vsName) const;

    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;

    // ----- accessors ------------------------------------------------------

    protected:
        /**
        * Specify the Class that uniquely identifies this metadata.
        *
        * @param vClz  class that defines this type
        */
        virtual void setClass(Class::View vClz);

        /**
        * Specify the TypeKey.
        *
        * @param vKey  unique identifier of this metadata instance
        */
        virtual void setKey(TypeKey::View vKey);

        /**
        * Specify all AttributeMetadata instances that represent this type.
        *
        * @param vColAttr  attribute metadata information to enrich this
        *                  metadata
        */
        virtual void setAttributes(Collection::View vColAttr);

        /**
         * Add an attribute to this TypeMetadata.
         *
         * @param vAttribute  attribute metadata definition to add
         *
         * @return whether the attribute metadata was added
         */
        virtual bool addAttribute(AttributeMetadata::Handle vAttribute);

    // ----- inner class: ClassKey ------------------------------------------

    public:
        /**
        * A ClassKey contains information to uniquely identify this class
        * type instance. Specifically unique identification is a product of
        * @code typeId + version + class-hash.@endcode.
        *
        * @author hr 2011.06.29
        *
        * @since 3.7.1
        */
        class COH_EXPORT ClassKey
            : public class_spec<ClassKey,
                       extends<Object>,
                       implements<TypeMetadata::TypeKey> >
            {
            friend class factory<ClassKey>;
            friend class ClassMetadataBuilder;

            // ----- TypeKey interface --------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual int32_t getTypeId() const;

                /**
                * {@inheritDoc}
                */
                virtual int32_t getVersionId() const;

                /**
                * {@inheritDoc}
                */
                virtual int32_t getHash() const;

            // ----- Object interface ---------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- accessors ----------------------------------------------

            protected:
                /**
                * Sets the type id of this type to uniquely identify this
                * type.
                *
                * @param nTypeId  type id of this type
                */
                virtual void setTypeId(int32_t nTypeId);

                /**
                * Sets the version of this type.
                *
                * @param nVersionId  version identifier
                */
                virtual void setVersionId(int32_t nVersion);

                /**
                * Sets the hash to uniquely identify a ClassMetadata
                * instance.
                *
                * @param nHash  hash of a ClassMetadata instance
                */
                virtual void setHash(int32_t nHash);

            // ----- data members -------------------------------------------

            private:
                /**
                * The POF user type id this key represents.
                */
                int32_t m_nTypeId;

                /**
                * The version of this type.
                */
                int32_t m_nVersionId;

                /**
                 * A hash that uniquely identifies the structure of this
                 * class.
                 */
                int32_t m_nHash;
            };

    // ----- inner class: ClassAttribute ------------------------------------

    public:
        /**
        * An AttributeMetadata implementation acting as a container for
        * attribute inspection and invocation.
        *
        * @author hr 2011.06.29
        *
        * @see AttributeMetadata
        *
        * @since 3.7.1
        */
        class COH_EXPORT ClassAttribute
            : public class_spec<ClassAttribute,
                      extends<Object>,
                      implements<TypeMetadata::AttributeMetadata,
                                 Comparable> >
            {
            friend class factory<ClassAttribute>;
            friend class ClassAttributeBuilder;
            friend class ClassMetadataBuilder;

            // ----- constructors -------------------------------------------

            protected:
                /**
                * Construct a ClassAttribute with the specified defaults.
                */
                ClassAttribute(String::View vsName   = String::null_string,
                               int32_t      nVersion = 0,
                               int32_t      nIndex   = -1);

            // ----- AttributeMetadata interface ----------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual String::View getName() const;

                /**
                * {@inheritDoc}
                */
                virtual int32_t getVersionId() const;

                /**
                * {@inheritDoc}
                */
                virtual int32_t getIndex() const;

                /**
                * {@inheritDoc}
                */
                virtual Codec::View getCodec() const;

                /**
                * {@inheritDoc}
                */
                virtual Object::Holder get(Object::View ovContainer) const;

                /**
                * {@inheritDoc}
                */
                virtual void set(Object::Holder ohContainer, Object::Holder ohValue) const;

            // ----- Comparable interface -----------------------------------

            public:
                /**
                 * Sorting of attributes is determined by:
                 * <ol>
                 *   <li>version</li>
                 *   <li>index</li>
                 *   <li>name</li>
                 * </ol>
                 *
                 * @param v  class attribute to compare against
                 *
                 * @return a  negative integer, zero, or a positive integer
                 *            as this object is less than, equal to, or
                 *            greater than the specified object.
                 */
                virtual int32_t compareTo(Object::View v) const;

            // ----- Object interface ---------------------------------------

            public:
                /**
                * {@inheritDoc}
                */
                virtual bool equals(Object::View v) const;

                /**
                * {@inheritDoc}
                */
                virtual size32_t hashCode() const;

                /**
                * {@inheritDoc}
                */
                virtual TypedHandle<const String> toString() const;

            // ----- accessors ----------------------------------------------

            protected:
                /**
                * Specify the attribute name.
                *
                * @param vsName  the normalized name of the attribute
                */
                virtual void setName(String::View vsName);

                /**
                * Specify the version.
                *
                * @param nVersionId  integer representing the version of this
                *                    attribute metadata
                */
                virtual void setVersionId(int32_t nVersionId);

                /**
                * Specify the index.
                *
                * @param nIndex  to identify this attribute's position in a
                *                sequence
                */
                virtual void setIndex(int32_t nIndex);

                /**
                * Specify the codec.
                *
                * @param vCodec  the Codec used to (de)serialize this
                *                attribute
                */
                virtual void setCodec(Codec::View vCodec);

                /**
                * Specify an InvocationStrategy.
                *
                * @param vStrategy  the invocation strategy to use to get and
                *                   set values
                */
                virtual void setInvocationStrategy(InvocationStrategy::View vStrategy);

            // ----- helpers ------------------------------------------------

            protected:
                /**
                * Ensures required state has been correctly set or throws an
                * IllegalStateException.
                *
                * @throws IllegalStateException if member variables m_class
                *         or m_invocationStrategy are null
                */
                virtual void assertState() const;

            // ----- data members -------------------------------------------

            private:
                /**
                * The unique name within this type representing this
                * attribute.
                */
                FinalView<String> f_vsName;

                /**
                * The version of this type information.
                */
                int32_t m_nVersionId;

                /**
                * Index to identify this attribute's position in a sequence.
                */
                int32_t m_nIndex;

                /**
                * A Codec allows reading/writing specific types from/to
                * PofReaders and PofWriters.
                */
                FinalView<Codec> f_vCodec;

                /**
                * An abstraction on how an attribute is invoked to return its
                * value or replace its value.
                */
                FinalView<InvocationStrategy> f_vStrategy;
            };

    // ----- data members ---------------------------------------------------

    private:
        /**
        * The Class used to inspect structural information.
        */
        MemberView<Class> m_vClz;

        /**
        * The unique identifier for this type.
        */
        FinalView<TypeKey> f_vKey;

        /**
        * All attributes within this type.
        */
        FinalHandle<Set> f_hSetAttr;

        /**
        * A reference store for efficient lookup from attribute name to
        * metadata.
        */
        FinalHandle<Map> f_hMapAttrByName;
    };

COH_CLOSE_NAMESPACE5

#endif // COH_CLASS_METADATA_HPP
