/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_PROPERTY_HPP
#define COH_PORTABLE_PROPERTY_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/reflect/Codec.hpp"

COH_OPEN_NAMESPACE4(coherence,io,pof,annotation)

/**
* A PortableProperty marks a method accessor as a POF serialized
* attribute. Whilst the PortableProperty::getIndex() and
* PortableProperty::getCodec() can be explicitly specified they can be
* determined by classes that use this annotation. Hence these attributes
* serve as hints to the underlying parser.
*
* @author hr  2011.06.29
*
* @since 3.7.1
*/
class COH_EXPORT PortableProperty
    : public class_spec<PortableProperty,
        extends<Object>,
        implements<Annotation> >
    {
    friend class factory<PortableProperty>;

    // ----- constructors ---------------------------------------------------

    protected:
        PortableProperty(int32_t nIndex = -1, Class::View vCodec = NULL);

    // ----- Annotation methods ---------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Class::View annotationType() const;

    // ----- PortableProperty methods ---------------------------------------

    public:
        /**
         * The index of this property.
         *
         * @see PofWriter
         *
         * @return POF index
         */
        virtual int32_t getIndex() const;

        /**
        * A codec to use to short-circuit determining the type via the
        * method return type.
        *
        * This could be used to determine concrete implementations of
        * interfaces, i.e. when the method return is a Map this type
        * definition could instruct the code to utilize a LinkedHashMap.
        *
        * @return class representing the codec to use for this property
        */
        virtual Class::View getCodec() const;

    // ----- static functions -----------------------------------------------

    public:
        /**
        * A convenience static method to return a reference to the Class
        * definition for this PortableProperty annotation class.
        */
        static Class::View getStaticClass();

    // ----- data members ---------------------------------------------------

    private:
        /**
        * The POF index used for serialization and deserialization by
        * PofAnnotationSerializer.
        */
        int32_t             m_nIndex;

        /**
        * A Class representing a Codec implementation.
        */
        FinalView<Class>   f_vClzCodec;
    };

COH_CLOSE_NAMESPACE4

#endif /* COH_PORTABLE_PROPERTY_HPP */
