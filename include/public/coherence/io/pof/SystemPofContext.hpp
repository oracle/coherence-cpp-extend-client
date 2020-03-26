/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYSTEM_POF_CONTEXT_HPP
#define COH_SYSTEM_POF_CONTEXT_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofAnnotationSerializer.hpp"
#include "coherence/io/pof/PortableObjectSerializer.hpp"
#include "coherence/io/pof/PortableTypeSerializer.hpp"
#include "coherence/io/pof/SimplePofContext.hpp"
#include "coherence/io/pof/TypedSerializer.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* System-wide PofContext implementation that allows POF user types to
* be registered programmatically.
*
* @author jh  2008.02.21
*/
class COH_EXPORT SystemPofContext
    : public cloneable_spec<SystemPofContext,
        extends<SimplePofContext> >
    {
    friend class factory<SystemPofContext>;

    // ----- constructors ---------------------------------------------------

    public:
        /**
        * @internal
        */
        using this_spec::create;

    private:
        /**
        * @internal
        */
        SystemPofContext();

        /**
        * Copy constructor.
        */
        SystemPofContext(const SystemPofContext& that);


    public:
        /**
        * Return the SystemPofContext singleton instance.
        *
        * @return the global SystemPofContext
        */
        static Handle getInstance();

        /**
        * Executable entrypoint for the SystemPofContext class.
        *
        * Print the POF registrations.
        *
        * @param vasArg  a list of libraries to load, and print their
        *                registered classes.
        */
        static void main(ObjectArray::View vasArg);
    };


// ----- helper macros ------------------------------------------------------

/**
* Register a PofSerializer with the system PofContext. The call will also
* register the class with the SystemClassLoader.
*
* @param POF_TYPE_ID     the POF type identifier
* @param CLASS           the associated class to register
* @param POF_SERIALIZER  the PofSerializer instance to register
*/
#define COH_REGISTER_POF_SERIALIZER(POF_TYPE_ID, CLASS, POF_SERIALIZER)     \
    COH_STATIC_INIT(coherence::io::pof::SystemPofContext::getInstance()->   \
        registerUserType(POF_TYPE_ID,                                       \
            coherence::lang::SystemClassLoader::getInstance()->             \
            registerClass(CLASS), POF_SERIALIZER))

/**
* Register a PortableObject implementation only with the system PofContext.
* The class must have already been registered with the SystemClassLoader.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_ONLY_PORTABLE_CLASS(POF_TYPE_ID, TYPE)                 \
    COH_STATIC_INIT_EX(1,                                                   \
        coherence::io::pof::SystemPofContext::getInstance()->               \
            registerUserType(POF_TYPE_ID, s_coh_static_class_##TYPE,        \
                coherence::io::pof::PortableObjectSerializer::create(POF_TYPE_ID)))

/**
* Register a PortableObject implementation with the system ClassLoader and
* PofContext.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_PORTABLE_CLASS(POF_TYPE_ID, TYPE)                      \
    COH_REGISTER_TYPED_CLASS(TYPE);                                         \
    COH_REGISTER_ONLY_PORTABLE_CLASS(POF_TYPE_ID, TYPE)

/**
* Register a Managed object implementation with the system ClassLoader and
* PofContext.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_MANAGED_CLASS(POF_TYPE_ID, TYPE)                       \
    COH_REGISTER_NAMED_CLASS(TYPE,                                          \
        coherence::lang::TypedClass<coherence::lang::Managed<TYPE > >::create()); \
    COH_STATIC_INIT_EX(1, coherence::io::pof::SystemPofContext::getInstance()->   \
        registerUserType(POF_TYPE_ID, s_coh_static_class_##TYPE,            \
        coherence::io::pof::TypedSerializer<coherence::lang::Managed<TYPE > >::create()))

/**
* Register a PortableObject implementation to use the PortableTypeSerializer
* only with the system PofContext. The class must have already been registered
* with the SystemClassLoader.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_ONLY_PORTABLE_TYPE_CLASS(POF_TYPE_ID, TYPE)            \
    COH_STATIC_INIT_EX(1,                                                   \
        coherence::io::pof::SystemPofContext::getInstance()->               \
        registerUserType(POF_TYPE_ID, s_coh_static_class_##TYPE,            \
        coherence::io::pof::PortableTypeSerializer::create(POF_TYPE_ID)))

/**
* Register a PortableObject to use the PortableTypeSerializer implementation
* with the system ClassLoader and PofContext.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_PORTABLE_TYPE_CLASS(POF_TYPE_ID, TYPE)                 \
    COH_REGISTER_TYPED_CLASS(TYPE);                                         \
    COH_REGISTER_ONLY_PORTABLE_TYPE_CLASS(POF_TYPE_ID, TYPE)

/**
* Register an annotated class with the system ClassLoader and PofContext. The
* Class (TYPE) must be registered with the SystemClassLoader, which is
* assumed based on this being a pre-requisite to adding annotations, and have
* a coherence::io::pof::Portable annotation present.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_POF_ANNOTATED_CLASS(POF_TYPE_ID, TYPE)                 \
    COH_STATIC_INIT(coherence::io::pof::SystemPofContext::getInstance()->   \
        registerUserType(POF_TYPE_ID,                                       \
            coherence::lang::SystemClassLoader::getInstance()->             \
            loadByType(typeid(TYPE)),                                       \
            coherence::io::pof::PofAnnotationSerializer::create(POF_TYPE_ID,\
                       coherence::lang::SystemClassLoader::getInstance()->  \
                           loadByType(typeid(TYPE)))))

/**
* Register an annotated class with the system ClassLoader and PofContext. The
* Class (TYPE) must be registered with the SystemClassLoader, which is
* assumed based on this being a pre-requisite to adding annotations, and have
* a coherence::io::pof::Portable annotation present. This macro additionally
* suggests to the serializer to determine portable property indexes.
*
* @param POF_TYPE_ID  the POF type identifier
* @param TYPE         the class type to register
*/
#define COH_REGISTER_POF_ANNOTATED_CLASS_AI(POF_TYPE_ID, TYPE)              \
    COH_STATIC_INIT(coherence::io::pof::SystemPofContext::getInstance()->   \
        registerUserType(POF_TYPE_ID,                                       \
            coherence::lang::SystemClassLoader::getInstance()->             \
            loadByType(typeid(TYPE)),                                       \
            coherence::io::pof::PofAnnotationSerializer::create(POF_TYPE_ID,\
                       coherence::lang::SystemClassLoader::getInstance()->  \
                           loadByType(typeid(TYPE)), true)))

COH_CLOSE_NAMESPACE3

#endif // COH_SYSTEM_POF_CONTEXT_HPP
