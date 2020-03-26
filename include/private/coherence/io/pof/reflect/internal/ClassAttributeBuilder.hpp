/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CLASS_ATTRIBUTE_BUILDER_HPP
#define COH_CLASS_ATTRIBUTE_BUILDER_HPP

#include "coherence/lang.ns"

#include "private/coherence/io/pof/reflect/internal/ClassMetadata.hpp"

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

/**
* The ClassAttributeBuilder provide the ability to build a
* AttributeMetadata implementation.
*
* @author hr 2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT ClassAttributeBuilder
    : public class_spec<ClassAttributeBuilder,
               extends<Object> >
    {
    friend class factory<ClassAttributeBuilder>;

    // ----- constructors ---------------------------------------------------

    public:
        ClassAttributeBuilder();

    // ----- ClassAttributeBuilder members ----------------------------------

    public:
        /**
        * Specify the normalized name of the ClassAttribute instance.
        *
        * @see ClassAttribute::getName()
        *
        * @param vsName  the normalized name of the ClassAttribute instance
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassAttributeBuilder::Handle setName(String::View vsName);

        /**
        * Specify the versionId of this ClassAttribute instance.
        *
        * @see ClassAttribute::getVersionId()
        *
        * @param  nVersionId  version of the ClassAttribute instance
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassAttributeBuilder::Handle setVersionId(int32_t nVersionId);

        /**
        * Specify the index of this ClassAttribute instance used to
        * sequence many ClassAttribute instances.
        *
        * @see ClassAttribute::getIndex()
        *
        * @param nIndex  index to specify this attributes sequence number
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassAttributeBuilder::Handle setIndex(int32_t nIndex);

        /**
        * Specify the Codec to use for this ClassAttribute instance.
        *
        * @see ClassAttribute::getCodec()
        *
        * @param vsCodec  the codec to use for this ClassAttribute instance
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassAttributeBuilder::Handle setCodec(Codec::View vsCodec);

        /**
        * Specify the InvocationStrategy implementation that allows
        * values to be written and received to the attribute.
        *
        * @see ClassAttribute::setInvocationStrategy(InvocationStrategy)
        *
        * @param vStrategy  the strategy provides an implementation to write
        *                   and receive values
        *
        * @return a reference to this for chained set calls
        */
        virtual ClassAttributeBuilder::Handle setInvocationStrategy(InvocationStrategy::View vStrategy);

        /**
        * Create a ClassAttribute instance based on the values set
        * during the lifetime of this builder.
        *
        * @return an enriched ClassAttribute instance
        */
        virtual ClassMetadata::ClassAttribute::Handle build();

    // ----- data members ---------------------------------------------------

    private:
        /**
        * ClassAttribute that is built across the duration of
        * ClassAttributeBuilder calls until it is returned via the
        * ::build() method.
        */
        MemberHandle<ClassMetadata::ClassAttribute> m_hAttribute;
    };

COH_CLOSE_NAMESPACE5

#endif /* COH_CLASS_ATTRIBUTE_BUILDER_HPP */
