/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_SERIALIZER_HPP
#define COH_TYPED_SERIALIZER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofSerializer.hpp"
#include "coherence/io/pof/PofWriter.hpp"

// ----- helper function prototypes -----------------------------------------

template<class T> extern void serialize(coherence::io::pof::PofWriter::Handle hOut, const T& t);
template<class T> extern T deserialize(coherence::io::pof::PofReader::Handle hIn);

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* A PofSerializer implementation that supports the serialization and
* deserialization of any class to and from a POF stream, by delegating the
* serialization logic to templated free functions.
*
* The following set of free functions must be defined for the templated type:
* <ul>
*   <li>template<> void serialize<T>(PofWriter::Handle, const T&)</li>
*   <li>template<> T deserialize(PofReader::Handle)</li>
* </ul>
*
* This class is well suited for use with the coherence::lang::Managed template
* wrapper class. Combining the two allows for the addition of serialization
* support to existing non-coherence classes.
*
* @see coherence::lang::Managed
* @author mf  2008.07.07
*/
template<class T>
class TypedSerializer
    : public class_spec<TypedSerializer<T>,
        extends<Object>,
        implements<PofSerializer> >
    {
    friend class factory<TypedSerializer<T> >;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new TypedSerializer for the user type with template
        * parameter type.
        *
        * @return the new TypedSerializer
        */
        TypedSerializer()
            {
            }


    // ----- PofSerializer interface ----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void serialize(PofWriter::Handle hOut, Object::View v) const
            {
            ::serialize<typename T::View>(hOut, cast<typename T::View>(v));
            hOut->writeRemainder(NULL);
            }

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder deserialize(PofReader::Handle hIn) const
            {
            TypedHolder<T> oh = ::deserialize<TypedHolder<T > >(hIn);
            hIn->readRemainder();
            return oh;
            }
    };

COH_CLOSE_NAMESPACE3

/**
* Default serialization implementation which delegates to the serialization
* for a boxed type. This implementation is targeted towards Managed objects
* essentially delegating to serialization functions for the non-managed custom
* type.
*
* @param hOut  the stream to write the object to
* @param t     the object to write out
*/
template<class T> void serialize(coherence::io::pof::PofWriter::Handle hOut, const T& t)
    {
    serialize<typename T::ValueType::BoxedType>(hOut, *t);
    }

/**
* Default deserialization implementation which delegates to the deserialization
* for a boxed type. This implementation is targeted towards Managed objects
* essentially delegating to serialization functions for the non-managed custom
* type.
*
* @param hIn  the stream to read the object from
*
* @return     the deserialized object
*/
template<class T> T deserialize(coherence::io::pof::PofReader::Handle hIn)
    {
    return T::ValueType::create(deserialize<typename T::ValueType::BoxedType>(hIn));
    }

#endif // COH_TYPED_SERIALIZER_HPP
