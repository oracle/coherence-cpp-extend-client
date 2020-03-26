/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SAFE_COMPARATOR_HPP
#define COH_SAFE_COMPARATOR_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Comparator.hpp"
#include "coherence/util/QueryMap.hpp"
#include "coherence/util/comparator/EntryAwareComparator.hpp"
#include "coherence/util/comparator/QueryMapComparator.hpp"

COH_OPEN_NAMESPACE3(coherence,util,comparator)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* NULL-safe delegating comparator. NULL values are evaluated as "less then"
* any non-null value. If the wrapped comparator is not specified then all
* non-null values must implement the coherence::util::Comparable interface.
* Use SafeComparator::getInstance to obtain an instance of non-delegating
* SafeComparator.
*
* @author djl  2008.05.05
*/
class COH_EXPORT SafeComparator
    : public class_spec<SafeComparator,
        extends<Object>,
        implements<Comparator,
                   QueryMapComparator,
                   EntryAwareComparator,
                   PortableObject> >
    {
    friend class factory<SafeComparator>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        SafeComparator();

        /**
        * Construct a SafeComparator delegating to the specified (wrapped)
        * comparator.
        *
        * @param vComparator  Comparator object to delegate comparison of
        *                     non-null values (optional)
        */
        SafeComparator(Comparator::View vComparator);

    private:
        /**
        * Blocked copy constructor.
        */
        SafeComparator(const SafeComparator&);


    // ----- Comparator interface -------------------------------------------

    public:
        /**
        * Compares its two arguments for order.  Returns a negative integer,
        * zero, or a positive integer as the first argument is less than, equal
        * to, or greater than the second. Null values are evaluated as "less
        * then" any non-null value. If the wrapped comparator is not specified,
        * all non-null values must implement the Comparable interface.
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


    // ----- QueryMap interface ---------------------------------------------

    public:
        /**
        * Compare two entries using the underlying comparator. If the wrapped
        * comparator does not implement the QueryMapComparator interface, revert
        * to the entry values comparison.
        */
        virtual int32_t compareEntries(QueryMap::Entry::View vEntry1,
                    QueryMap::Entry::View vEntry2) const;


    // ----- EntryAwareComparator interface ---------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool isKeyComparator() const;

        /**
        * Check whether the specified comparator expects to compare keys
        * or values.
        *
        * @param vComparator  a Comparator to check
        *
        * @return true if the comparator expects keys; false otherwise
        */
        static bool isKeyComparator(Comparator::View vComparator);


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

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the wrapped Comparator.
        *
        * @return the wrapped Comparator
        */
        virtual Comparator::View getComparator() const;


    // ----- Helpers --------------------------------------------------------

    public:
        /**
        * Compares its two arguments for order.  Returns a negative integer,
        * zero, or a positive integer as the first argument is less than, equal
        * to, or greater than the second. Null values are evaluated as
        * "less then" any non-null value. Non-null values must implement the
        * coherence::util::Comparable interface.
        *
        * @param vComparator  a comparator to use for the comparison (optional)
        * @param vO1          the first object to be compared
        * @param vO2          the second object to be compared
        *
        * @return a negative integer, zero, or a positive integer as the first
        *         argument is less than, equal to, or greater than the second
        *
        * @throws ClassCastException if the arguments are not Comparable
        */
        static int32_t compareSafe(Comparator::View vComparator,
                Object::View vO1, Object::View vO2);


    // ----- constants ------------------------------------------------------

    public:
        /**
        * An instance of the SafeComparator.
        */
        static SafeComparator::Handle getInstance();


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The wrapped Comparator. Could be null.
        */
        FinalView<Comparator> f_vComparator;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SAFE_COMPARATOR_HPP
