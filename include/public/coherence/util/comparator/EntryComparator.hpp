/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ENTRY_COMPARATOR_HPP
#define COH_ENTRY_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/comparator/SafeComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Comparator implementation used to compare map entries. Depending on the
* comparison style this comparator will compare entries' values, entries'
* keys or, when the provided comparator is an instance of
* QueryMapComparator, the entries themselves.
*
* @author nsa 2008.05.30
*/
class COH_EXPORT EntryComparator
    : public class_spec<EntryComparator,
        extends<SafeComparator> >
    {
    friend class factory<EntryComparator>;

    // ----- constants ------------------------------------------------------

    public:
        /**
        * Indicates that this EntryComparator should choose the comparison
        * style based on the underying comparator type.
        */
        static const int32_t cmp_auto = 0;

        /**
        * Indicates that this EntryComparator should compare the entries'
        * values.
        */
        static const int32_t cmp_value = 1;

        /**
        * Indicates that this EntryComparator should compare the entries'
        * keys.
        */
        static const int32_t cmp_key   = 2;

        /**
        * Indicates that entries that implement
        * {@link com.tangosol.util.QueryMap.Entry} interface will be compared
        * using the QueryMapComparator#compareEntries
        * method.
        */
        static const int32_t cmp_entry = 3;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        EntryComparator();

        /**
        * Construct a SafeComparator delegating to the specified (wrapped)
        * comparator.
        *
        * @param vComparator  Comparator object to delegate comparison of
        *                     non-null values (optional)
        */
        EntryComparator(Comparator::View vComparator, int32_t nStyle = cmp_auto);

    private:
        /**
        * Blocked copy constructor.
        */
        EntryComparator(const EntryComparator&);


    // ----- Comparator interface -------------------------------------------

    public:
        /**
        * Compares two arguments for order. The arguments must be
        * coherence::util::Map::Entry objects. Depending on the comparison
        * style, this method will pass either the entries' values, keys or
        * the entries themselves to the underlying Comparator.
        *
        * @param vO1  the first object to be compared
        * @param vO2  the second object to be compared
        *
        * @return a negative integer, zero, or a positive integer as the first
        *         argument is less than, equal to, or greater than the second
        *
        * @throws ClassCastException if the arguments' types prevent them from
        *         being compared by this Comparator.
        */
        virtual int32_t compare(Object::View vO1, Object::View vO2) const;


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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Obtain the comparison style value utilized by this EntryComparator. The
        * returned value should be one of the CMP_* constants.
        *
        * @return  the comparison style value
        */
        int32_t getComparisonStyle();

        /**
        * Check whether or not this EntryComparator uses entries' values to pass
        * for comparison to the underlying Comparator.
        *
        * @return true iff entries' values are used for comparison
        */
        bool isCompareValue();

        /**
        * Check whether or not this EntryComparator uses entries' keys to pass
        * for comparison to the underlying Comparator.
        *
        * @return true iff entries' keys are used for comparison
        */
        bool isCompareKey();

        /**
        * Check whether or not this EntryComparator pass entries themselves for
        * comparison to the underlying
        * QueryMapComparator#compareEntries method.
        *
        * @return true iff entries themselves are used for comparison
        */
        bool isCompareEntry();


    // ----- data members  --------------------------------------------------

    protected:
        /**
        * Comparison style utilized by this EntryComparator. Valid values are any
        * of the CMP_* constants.
        */
        int32_t m_nStyle;
    };


COH_CLOSE_NAMESPACE3

#endif /*COH_ENTRY_COMPARATOR_HPP*/
