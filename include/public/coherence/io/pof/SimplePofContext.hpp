/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_POF_CONTEXT_HPP
#define COH_SIMPLE_POF_CONTEXT_HPP

#include "coherence/lang.ns"

#include "coherence/io/ReadBuffer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/io/pof/PofContext.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/util/LongArray.hpp"
#include "coherence/util/Map.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::ReadBuffer;
using coherence::io::WriteBuffer;
using coherence::util::LongArray;
using coherence::util::Map;


/**
* Basic PofContext implementation.
*
* @author gm  2008.10.24
*
* @see PortableObjectSerializer
*/
class COH_EXPORT SimplePofContext
    : public cloneable_spec<SimplePofContext,
        extends<Object>,
        implements<PofContext> >
    {
    friend class factory<SimplePofContext>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * @internal
        */
        SimplePofContext();

        /**
        * Copy constructor.
        */
        SimplePofContext(const SimplePofContext& that);


    // ----- SimplePofContext interface -------------------------------------

    public:
        /**
        * Associate a user type with a type identifier and
        * {@link PofSerializer}.
        *
        * @param nTypeId      the type identifier of the specified user type;
        *                     must be greater or equal to 0
        * @param vClass       the user type to register with this PofContext;
        *                     must not be NULL
        * @param vSerializer  the PofSerializer that will be used to
        *                     serialize and deserialize objects of the
        *                     specified class; must not be NULL
        *
        * @throws coherence::lang::IllegalArgumentException on invalid type
        *         identifer, class, or PofSerializer
        */
        virtual void registerUserType(int32_t nTypeId, Class::View vClass,
                PofSerializer::View vSerializer);

        /**
        * Unregister a user type previously registered using the specified
        * type identifier.
        *
        * @param nTypeId  the type identifier of the user type to unregister
        *
        * @throws coherence::lang::IllegalArgumentException  if the specified
        *         user type identifier is unknown to this PofContext
        */
        virtual void unregisterUserType(int32_t nTypeId);


    // ----- PofContext interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual PofSerializer::View getPofSerializer(int32_t nTypeId) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeIdentifier(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeIdentifier(Class::View vClass) const;

        /**
        * {@inheritDoc}
        */
        virtual int32_t getUserTypeIdentifier(String::View vsClass) const;

        /**
        * {@inheritDoc}
        */
        virtual String::View getClassName(int32_t nTypeId) const;

        /**
        * {@inheritDoc}
        */
        virtual Class::View getClass(int32_t nTypeId) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isUserType(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isUserType(Class::View vClass) const;

        /**
        * {@inheritDoc}
        */
        virtual bool isUserType(String::View vsClass) const;


    // ----- Serializer interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void serialize(WriteBuffer::BufferOutput::Handle hOut,
                Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder deserialize(
                ReadBuffer::BufferInput::Handle hIn) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- internal methods -----------------------------------------------

    protected:
        /**
        * Ensure that the given user type identifier is valid.
        *
        * @param nTypeId  the user type identifier to validate
        *
        * @throws coherence::lang::IllegalArgumentException if the given user
        *         type identifier is negative
        */
        virtual void validateTypeId(int32_t nTypeId) const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Determine if Identity/Reference type support is enabled for this
        * SimplePofContext.
        *
        * @return true if Identity/Reference type support is enabled
        *
        * @since Coherence 3.7.1
        */
        virtual bool isReferenceEnabled() const;

        /**
        * Enable or disable POF Identity/Reference type support for this
        * SimplePofContext.
        *
        * @param fEnabled  true to enable POF Identity/Reference type support;
        *                  false to disable
        *
        * @since Coherence 3.7.1
        */
        virtual void setReferenceEnabled(bool fEnabled);

        /**
        * Enable or disable POF Identity/Reference type support for this
        * SimplePofContext.
        *
        * @param fEnabled  true to enable POF Identity/Reference type support;
        *                  false to disable
        *
        * @since Coherence 3.7.1
        */
        virtual void setReferenceEnabled(bool fEnabled) const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * A LongArray of user types, indexed by type identifier.
        */
        FinalHandle<LongArray> f_hlaClass;

        /**
        * A LongArray of PofSerializer objects, indexed by type identifier.
        */
        FinalHandle<LongArray> f_hlaSerializer;

        /**
        * A Map that contains mappings from a registered class name to type
        * identifier.
        */
        FinalHandle<Map> f_hMapClassName;

        /**
        * True if POF Identity/Reference type support is enabled.
        */
        mutable bool m_fReferenceEnabled;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SIMPLE_POF_CONTEXT_HPP
