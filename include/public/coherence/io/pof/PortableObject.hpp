/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_PORTABLE_OBJECT_HPP
#define COH_PORTABLE_OBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)


/**
* The PortableObject interface is implemented by classes that can
* self-serialize and deserialize their state to and from a POF data stream.
*
* The {@link #readExternal()} and {@link #writeExternal()} methods of the
* PortableObject interface are implemented by a class to give the class
* complete control its own POF serialization and deserialization.
*
* @author jh  2008.01.14
*/
class COH_EXPORT PortableObject
    : public interface_spec<PortableObject>
    {
    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * Restore the contents of a user type instance by reading its state
        * using the specified PofReader.
        *
        * @param hIn  the PofReader from which to read the object's state
        */
        virtual void readExternal(PofReader::Handle hIn) = 0;

        /**
        * Save the contents of a POF user type instance by writing its state
        * using the specified PofWriter.
        *
        * @param hOut  the PofWriter to which to write the object's state
        */
        virtual void writeExternal(PofWriter::Handle hOut) const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_PORTABLE_OBJECT_HPP
