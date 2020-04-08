/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_CONTEXT_HPP
#define COH_POF_CONTEXT_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/pof/PofSerializer.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::io::Serializer;


/**
* A PofContext is a registry of PofSerializer instances, and thus, represents
* a set of user types that can be serialized to and deserialized from a POF
* stream.
*
* @author jh  2008.01.18
*/
class COH_EXPORT PofContext
    : public interface_spec<PofContext,
        implements<Serializer> >
    {
    // ----- PofContext interface -------------------------------------------

    public:
        /**
        * Return a PofSerializer that can be used to serialize and
        * deserialize an objectof the specified user type to and from a POF
        * stream.
        *
        * @param nTypeId  the type identifier of the user type that can be
        *                 serialized and deserialized using the returned
        *                 PofSerializer
        *
        * @return a PofSerializer for the specified user type
        *
        * @throws coherence::lang::IllegalArgumentException if the specified
        *         user type is negative or unknown to this PofContext
        */
        virtual PofSerializer::View getPofSerializer(int32_t nTypeId) const = 0;

        /**
        * Determine the user type identifier associated with the given
        * object.
        *
        * @param v  an instance of a user type; must not be NULL
        *
        * @return the type identifier of the user type associated with the
        *         given object
        *
        * @throws coherence::lang::IllegalArgumentException if the user type
        *         associated with the given object is unknown to this
        *         PofContext or if the object is NULL
        */
        virtual int32_t getUserTypeIdentifier(Object::View v) const = 0;

        /**
        * Determine the user type identifier associated with the given class.
        *
        * @param vClass  a user type class; must not be NULL
        *
        * @return the type identifier of the user type associated with the
        *         given class
        *
        * @throws coherence::lang::IllegalArgumentException if the user type
        *         associated with the given class is unknown to this
        *         PofContext or if the class is NULL
        */
        virtual int32_t getUserTypeIdentifier(Class::View vClass) const = 0;

        /**
        * Determine the user type identifier associated with the given class
        * name.
        *
        * @param vsClass  the name of a user type class; must not be NULL
        *
        * @return the type identifier of the user type associated with the
        *         given class name
        *
        * @throws coherence::lang::IllegalArgumentException if the user type
        *         associated with the given class name is unknown to this
        *         PofContext or if the class name is NULL
        */
        virtual int32_t getUserTypeIdentifier(String::View vsClass) const = 0;

        /**
        * Determine the name of the class associated with the given user type
        * identifier.
        *
        * @param nTypeId  the user type identifier; must be non-negative
        *
        * @return the name of the class associated with the specified user
        *         type identifier
        *
        * @throws coherence::lang::IllegalArgumentException if the specified
        *         user type is negative or unknown to this PofContext
        */
        virtual String::View getClassName(int32_t nTypeId) const = 0;

        /**
        * Determine the class associated with the given user type identifier.
        *
        * @param nTypeId  the user type identifier; must be non-negative
        *
        * @return the class associated with the specified user type
        *         identifier
        *
        * @throws coherence::lang::IllegalArgumentException if the specified
        *         user type is negative or unknown to this PofContext
        */
        virtual Class::View getClass(int32_t nTypeId) const = 0;

        /**
        * Determine if the given object is an instance of a user type known
        * to this PofContext.
        *
        * @param v  the object to test; must not be NULL
        *
        * @return true iff the specified object is of a valid user type
        *
        * @throws coherence::lang::IllegalArgumentException if the given
        *         object is NULL
        */
        virtual bool isUserType(Object::View v) const = 0;

        /**
        * Determine if the given class is a user type known to this
        * PofContext.
        *
        * @param vClass  the class to test; must not be NULL
        *
        * @return true iff the specified class is a valid user type known to
        *         this PofContext
        *
        * @throws coherence::lang::IllegalArgumentException if the given
        *         class is NULL
        */
        virtual bool isUserType(Class::View vClass) const = 0;

        /**
        * Determine if the class with the given name is a user type known to
        * this PofContext.
        *
        * @param vsClass  the name of the class to test; must not be NULL
        *
        * @return true iff the class with the specified name is a valid user
        *         type
        *
        * @throws coherence::lang::IllegalArgumentException if the given
        *         class name is NULL
        */
        virtual bool isUserType(String::View vsClass) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_POF_CONTEXT_HPP
