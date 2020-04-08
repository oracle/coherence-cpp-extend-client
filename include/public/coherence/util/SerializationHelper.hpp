/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERIALIZATION_HELPER_HPP
#define COH_SERIALIZATION_HELPER_HPP

#include "coherence/lang.ns"

#include "coherence/io/Serializer.hpp"
#include "coherence/io/WriteBuffer.hpp"
#include "coherence/util/Binary.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::Serializer;
using coherence::io::WriteBuffer;


/**
* Miscellaneous serialization utilities.
*
* @author phf  2011.04.19
*
* @since Coherence 3.7.1
*/
class COH_EXPORT SerializationHelper
    : public abstract_spec<SerializationHelper>
    {
    // ----- constructors ---------------------------------------------------

    private:
        /**
        * This constructor is blocked (private) as SerializationHelper
        * provides only a static interface and no instances may be created.
        */
        SerializationHelper();


    // ----- SerializationHelper interface ----------------------------------

    public:
        /**
        * Decorate the specified Binary value with the specified integer
        * decoration.
        *
        * @param vBinValue     the Binary value to be decorated
        * @param nDecoration   the integer decoration
        *
        * @return the decorated (with integer decoration) Binary object
        */
        static Binary::View decorateBinary(Binary::View vBinValue, int32_t nDecoration);

        /**
        * Extract a decoration value from the specified Binary that contains
        * an int decoration
        *
        * @param vBin  the Binary object
        *
        * @return the integer decoration value
        *
        * @throws IllegalArgumentException if the Binary does not have an
        *         int decoration
        */
        static int32_t extractIntDecoration(Binary::View vBin);

        /**
        * Check whether or not the specified Binary has an integer
        * decoration.
        *
        * @param vBin  the Binary object
        *
        * @return true if the Binary contains (starts with) an integer
        *         decoration; false otherwise
        */
        static bool isIntDecorated(Binary::View vBin);

        /**
        * Remove a decoration value from the specified Binary that contains a
        * representation of an IntDecoratedObject.
        *
        * @param vBin  the Binary object
        *
        * @return the undecorated Binary value
        */
        static Binary::View removeIntDecoration(Binary::View vBin);

        /**
        * Serialize an object into its Binary form.
        *
        * @param v            object to serialize
        * @param vSerializer  Serializer to use
        *
        * @return Binary representation of the specified object
        */
        static Binary::View toBinary(Object::View v, Serializer::View vSerializer);

        /**
        * Deserialize an object from its Binary form.
        *
        * @param vBin         Binary representation of an object
        * @param vSerializer  Serializer to use
        *
        * @return deserialized object
        *
        * @throws IOException if an I/O error occurs
        */
        static Object::Holder fromBinary(Binary::View vBin, Serializer::View vSerializer);


    // ----- inner class: Stats ---------------------------------------------

    protected:
        /**
        * Serialization statistics for a given user type.
        */
        class COH_EXPORT Stats
            : public class_spec<Stats,
                extends<Object> >
            {
            friend class factory<Stats>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Create a new Stats.
                */
                Stats();


            // ----- Stats methods --------------------------------------

            public:
                /**
                * Update the serialization statistics with the size (in
                * bytes) of a newly serialized object.
                *
                * @param cb  the number of bytes used to serialize
                */
                virtual void update(int32_t cb);

                /**
                * Instantiate a WriteBuffer to write a user type for which
                * this Stats object maintains serialization statistics.
                *
                * @return a WriteBuffer to write to
                */
                virtual WriteBuffer::Handle instantiateBuffer() const;


            // ----- constants ------------------------------------------

            private:
                /**
                * The expiry for statistics (in milliseconds).
                */
                static const int32_t expiry_millis = 10 * 60 * 1000; // 10 minutes


            // ----- data members ---------------------------------------

            private:
                /**
                * <ul>
                * <li>high 2 bytes - Number of items that have been
                * submitted for statistics keeping.</li>
                * <li>low 6 bytes - Total number of bytes of all the items
                * submitted.</li>
                * </ul>
                */
                Volatile<int64_t> m_lAccum;

                /**
                * <ul>
                * <li>highWord - Largest size in bytes of all the items
                * submitted.</li>
                * <li>lowWord  - The average size in bytes of all the items
                * submitted.</li>
                * </ul>
                */
                Volatile<int64_t> m_lStats;

                /**
                * Time at which this Stats object was created.
                */
                Volatile<int64_t> m_ldtCreated;
            };


    // ----- helper methods -------------------------------------------------

    private:
        /**
        * Calculate a somewhat unique ID for the type of the passed Object.
        *
        * @param v  a user type value
        *
        * @return an ID that is hopefully unique across the set of user type
        *         classes in use within this process at this general point in
        *         time
        */
        static int32_t calculateStatsId(Object::View v);

        /**
        * If statistics are being maintained for the class of the specified
        * Object value, then find and return those stats.
        *
        * @param v  the value to search for a Stats object for
        *
        * @return the Stats object for the specified Object value, or NULL
        */
        static SerializationHelper::Stats::Handle findStats(Object::View v);

        /**
        * If statistics are being maintained for the class of the specified
        * Object value, then find and return those stats.
        *
        * @param v      the object that has been written
        * @param stats  the statistics that track the serialized sizes of
        *               objects
        * @param cb     the size in bytes of the object as it was written
        */
        static void updateStats(Object::View v,
                SerializationHelper::Stats::Handle hStats, int32_t cb);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Decoration: The original value (before being decorated).
        */
        static const int32_t deco_value   = 0;

        /**
        * Serialization format: Decorated Binary value.
        *
        * Structure is:
        * <pre>
        * byte 0    : format identifier (18)
        * byte 1    : bit mask of decoration identifiers (see DECO_* constants)
        * byte 2    : packed int specifying the length of the first decoration
        * byte next : binary data
        * ...
        * </pre>
        * For each decoration, there is a packed int for its length, followed by
        * its binary data. The first decoration is the decorated value itself, if
        * present.
        *
        * Note: fmt_ido cannot be combined with fmt_bin_deco.
        */
        static const octet_t fmt_bin_deco = 18;

        /**
        * Serialization format: Extended-range Decorated Binary value.
        *
        * Structure is:
        * <pre>
        * byte 0    : format identifier (19)
        * byte 1    : bit mask of decoration identifiers (see DECO_* constants),
        *             in the packed long format (1-10 bytes)
        * byte next : packed int specifying the length of the first decoration
        * byte next : binary data
        * ...
        * </pre>
        * For each decoration, there is a packed int for its length, followed by
        * its binary data. The first decoration is the decorated value itself, if
        * present.
        *
        * Note: fmt_ido cannot be combined with fmt_bin_ext_deco.
        */
        static const octet_t fmt_bin_ext_deco = 19;

        /**
        * Serialization format: A DefaultSerializer is NOT used.
        */
        static const octet_t fmt_ext = 21;

        /**
        * Serialization format: Integer-decorated value.
        */
        static const octet_t fmt_ido = 13;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERIALIZATION_HELPER_HPP
