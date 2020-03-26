/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CONDITIONAL_INDEX_HPP
#define COH_CONDITIONAL_INDEX_HPP

#include "coherence/lang.ns"

#include "coherence/util/Comparator.hpp"
#include "coherence/util/Filter.hpp"
#include "coherence/util/Map.hpp"
#include "coherence/util/SimpleMapIndex.hpp"
#include "coherence/util/ValueExtractor.hpp"

COH_OPEN_NAMESPACE2(coherence,util)


/**
* ConditionalIndex is a conherence::util::MapIndex implementation that uses an associated
* filter to evaluate whether or not an entry should be indexed.  An entry's
* extracted value is only added to the index if the filter evaluates to true.
*
* @author tb/lh  2010.08.31
* @since Coherence 3.6.1
*/
class COH_EXPORT ConditionalIndex
    : public class_spec<ConditionalIndex,
        extends<SimpleMapIndex> >
    {
    friend class factory<ConditionalIndex>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a ConditionalIndex.
        *
        * @param vFilter        the filter that is used to evaluate the entries of
        *                       the resource map that is being indexed
        * @param vExtractor     the coherence::util::ValueExtractor that is used to extract
        *                       an indexed value from a resource map entry
        * @param fOrdered       true iff the contents of the indexed information
        *                       should be ordered; false otherwise
        * @param vComparator    the Comparator object which imposes an ordering on
        *                       entries in the index map; or <tt>null</tt> if the
        *                       entries' values natural ordering should be used
        * @param fForwardIndex  specifies whether or not this index supports a
        *                       forward map
        */
        ConditionalIndex(Filter::View vFilter,
            ValueExtractor::View vExtractor,
            bool fOrdered,
            Comparator::View vComparator,
            bool fForwardIndex);


    // ----- SimpleMapIndex methods -----------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual bool isPartial() const;


    protected:
        /**
        * {@inheritDoc}
        */
        virtual Map::Entry::View getForwardEntry(Object::View vKey) const;

        /**
        * {@inheritDoc}
        */
        virtual Map::Entry::Handle getForwardEntry(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        virtual void removeForwardEntry(Object::View vKey);

        /**
        * {@inheritDoc}
        */
        Map::Handle instantiateForwardIndex() const;


    // ----- helpers --------------------------------------------------------

    public:
        /**
        * Get the associated filter.
        *
        * @return the filter
        */
        Filter::View getFilter() const;

        /**
        * Determine whether or not this ConditionalIndex supports a forward index.
        *
        * @return true if this ConditionalIndex supports a forward index;
        *         false otherwise
        */
        bool isForwardIndexSupported() const;

    protected:
        /**
        * Evaluate the given entry using this index's filter.  If the entry does
        * not pass the filter then it should be excluded from this index, making
        * this a partial index.
        *
        * @param vEntry  the entry to evaluate
        *
        * @return true if the entry passes the filter; false otherwise
        */
        bool evaluateEntry(Map::Entry::View vEntry);

        /**
        * {@inheritDoc}
        */
        virtual void insertInternal(Map::Entry::View vEntry);

        /**
        * {@inheritDoc}
        */
        virtual void updateInternal(Map::Entry::View vEntry);

        /**
        * {@inheritDoc}
        */
        virtual void removeInternal(Map::Entry::View vEntry);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Writes a string representation of this ConditionalIndex to the stream.
        * The string representation consists of the SimpleMapIndex representaion
        * concatenated by the Filter and the ForwardIndexSupported flag.
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * Compares the specified object with this index for equality. Returns
        * <tt>true</tt> if the given object is also a SimpleMapIndex and the two
        * represent the same index.
        *
        * @param o object to be compared for equality with this MapIndex
        *
        * @return <tt>true</tt> if the specified object is equal to this index
        */
        virtual bool equals(Object::View v) const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Filter object that this index uses to evaluate entries.
        * An entry's extracted value is only added to the index if this filter
        * evaluates to true.
        */
        FinalView<Filter> f_vFilter;

        /**
        * Specifies whether or not this ConditionalIndex supports a forward index.
        */
        bool m_fForwardIndex;

        /**
        * Specifies whether or not this ConditionalIndex is a partial index.
        * The index is regarded as partial if any entry in the indexed map has
        * been excluded from this index.
        */
        bool m_fPartial;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CONDITIONAL_INDEX_HPP
