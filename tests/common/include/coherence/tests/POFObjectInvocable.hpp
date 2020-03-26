/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_OBJECT_INVOCABLE_HPP
#define COH_POF_OBJECT_INVOCABLE_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

#include "coherence/net/AbstractInvocable.hpp"

COH_OPEN_NAMESPACE2(coherence,tests)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::AbstractInvocable;

/**
* Invocable implementation that serializes and deserializes a POF object
* between c++ client and server
*
* @author wl  2010.08.27
*/
class POFObjectInvocable
    : public class_spec<POFObjectInvocable,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<POFObjectInvocable>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor - needed for PortableObject.
        */
        POFObjectInvocable();

        /**
        * Create a new POFObjectInvocable instance.
        *
        * @param v  the object to serialize
        */
        POFObjectInvocable(Object::View v);

    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * {@inheritDoc}
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;

    // ----- data members ---------------------------------------------------

    private:
        /**
        * The POF object.
        */
        FinalView<Object> f_vPOFObject;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_POF_OBJECT_INVOCABLE_HPP
