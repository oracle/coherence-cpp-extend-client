/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EVOLVABLE_HPP
#define COH_EVOLVABLE_HPP

#include "coherence/lang.ns"

// ----- forward declarations -----------------------------------------------

COH_OPEN_NAMESPACE2(coherence,util)

class Binary;

COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,io)


/**
* The Evolvable interface is implemented by classes that require forwards-
* and backwards-compatibility of their serialized form. An Evolvable class
* has an integer version identifier <tt>n</tt>, where <tt>n >= 0</tt>. When
* the contents and/or semantics of the serialized form of the Evolvable class
* changes, the version identifier is increased. Two versions identifiers,
* <tt>n1</tt> and <tt>n2</tt>, indicate the same version iff
* <tt>n1 == n2</tt>; the version indicated by <tt>n2</tt> is newer than the
* version indicated by <tt>n1</tt> iff <tt>n2 &gt; n1</tt>.
*
* The Evolvable interface is designed to support the evolution of classes by
* the addition of data. Removal of data cannot be safely accomplished as long
* as a previous version of the class exists that relies on that data.
* Modifications to the structure or semantics of data from previous versions
* likewise cannot be safely accomplished as long as a previous version of the
* class exists that relies on the previous structure or semantics of the
* data.
*
* When an Evolvable is deserialized, it retains any unknown data that has
* been added to newer versions of the class, and the version identifier for
* that data format. When the Evolvable is subsequently serialized, it
* includes both that version identifier and the unknown future data.
*
* When an Evolvable is deserialized from a data stream whose version
* identifier indicates an older version, it must default and/or calculate the
* values for any data fields and properties that have been added since that
* older version. When the Evolvable is subsequently serialized, it includes
* its own version identifier and all of its data. Note that there will be no
* unknown future data in this case; future data can only exist when the
* version of the data stream is newer than the version of the Evolvable
* class.
*
* @author jh  2008.01.08
*/
class COH_EXPORT Evolvable
    : public interface_spec<Evolvable>
    {
    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Binary View definition.
        */
        typedef TypedHandle<const coherence::util::Binary> BinaryView;


    // ----- Evolvable interface --------------------------------------------

    public:
        /**
        * Determine the serialization version supported by the implementing
        * class.
        *
        * @return the serialization version supported by this Evolvable
        */
        virtual int32_t getImplVersion() const = 0;

        /**
        * Obtain the version associated with the data stream from which this
        * Evolvable was deserialized. If the Object was constructed (not
        * deserialized), the data version is the same as the implementation
        * version.
        *
        * @return the version of the data used to initialize this Evolvable,
        *         greater than or equal to zero
        */
        virtual int32_t getDataVersion() const = 0;

        /**
        * Set the version associated with the data stream with which this
        * Evolvable is being deserialized.
        *
        * @param nVersion  the version of the data in the data stream that
        *                  will be used to deserialize this Object; greater
        *                  than or equal to zero
        */
        virtual void setDataVersion(int32_t nVersion) = 0;

        /**
        * Return all the unknown remainder of the data stream from which
        * this Evolvable was deserialized. The remainder is unknown because
        * it is data that was originally written by a future version of this
        * Evolvable's class.
        *
        * @return future data in binary form
        */
        virtual BinaryView getFutureData() const = 0;

        /**
        * Store the unknown remainder of the data stream from which this
        * Evolvable is being deserialized. The remainder is unknown because
        * it is data that was originally written by a future version of this
        * Evolvable's class.
        *
        * @param vBinFuture  future data in binary form
        */
        virtual void setFutureData(BinaryView vBinFuture) = 0;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EVOLVABLE_HPP
