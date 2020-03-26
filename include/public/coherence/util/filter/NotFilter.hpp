/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_NOT_FILTER_HPP
#define COH_NOT_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

/**
* Provide for "pluggable" conditional behavior that negates.
*
* @author djl  2008.03.02
*/
class COH_EXPORT NotFilter
    : public class_spec<NotFilter,
        extends<Object>,
        implements<IndexAwareFilter, PortableObject> >
    {
    friend class factory<NotFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        NotFilter();

        /**
        * Create and return a Handle to a new Object instance.
        */
        NotFilter(Filter::View hFilter);


    // ----- Entry Filter interface -----------------------------------------

    public:
        /**
        * Apply the test to a Map::Entry and negate the result.
        *
        * @param vEntry  the Map::Entry to evaluate; never NULL
        *
        * @return false if the test passes, true otherwise
        */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * Apply the test to the object and negate the result.
        *
        * @param v  the object to test
        *
        * @return true if the test passes, false otherwise
        */
        virtual bool evaluate(Object::View v) const;


    // ----- IndexAwareFilter interface -------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t calculateEffectiveness(Map::View vMapIndexes,
                Set::View vSetKeys) const;

        /**
        * {@inheritDoc}
        */
        virtual Filter::View applyIndex(Map::View vMapIndexes,
                Set::Handle hSetKeys) const;


    // ----- helpers --------------------------------------------------------

    protected:
        /**
        * Get a Map of the available non-partial indexes from the given Map of all
        * available indexes.
        *
        * @param vMapIndexes  the available MapIndex objects keyed by the
        *                     related ValueExtractor; read-only
        *
        * @return a Map of the available non-partial MapIndex objects
        */
        virtual Map::View getNonPartialIndexes(Map::View vMapIndexes) const;


    // ----- PortableObject interface ---------------------------------------

    public:
        /**
        * Restore the contents of a user type instance by reading its state
        * using the specified PofReader.
        *
        * @param hIn  the PofReader from which to read the object's state
        */
        virtual void readExternal(PofReader::Handle hIn);

        /**
        * Save the contents of a POF user type instance by writing its state
        * using the specified PofWriter.
        *
        * @param hOut  the PofWriter to which to write the object's state
        */
        virtual void writeExternal(PofWriter::Handle hOut) const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Compare the NotFilter with another object to determine equality.
        *
        * @param v  the object to compare with
        *
        * @return true iff this NotFilter and the passed object are equivalent
        *         NotFilter objects
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * Output a human-readable description of this Object to the given
        * stream.
        *
        * @param out  the stream used to output the description
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Obtain the Filter whose results are negated by this filter.
        *
        * @return the filter whose results are negated by this filter
        */
        virtual Filter::View getFilter() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The Filter whose results are negated by this filter.
        */
        FinalView<Filter> f_vFilter;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_NOT_FILTER_HPP
