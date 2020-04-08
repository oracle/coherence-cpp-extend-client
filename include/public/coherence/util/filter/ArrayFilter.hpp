/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ARRAY_FILTER_HPP
#define COH_ARRAY_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Filter which is a logical operator of a filter array.
*
* @author djl  2008.03.23
*/
class COH_EXPORT ArrayFilter
    : public abstract_spec<ArrayFilter,
        extends<Object>,
        implements<IndexAwareFilter, PortableObject> >
    {
    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        ArrayFilter();

        /**
        * Construct a logical filter that applies a binary operator to a
        * filter array. The result is defined as:
        *   vaFilter[0] &lt;op&gt; vaFilter[1] ... &lt;op&gt; vaFilter[n]
        *
        * @param vaFilter  the filter array
        */
        ArrayFilter(ObjectArray::View vaFilter);

    private:
        /**
        * Blocked copy constructor.
        */
        ArrayFilter(const ArrayFilter&);


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
        * Compare the ArrayFilter with another object to determine
        * equality. Two ArrayFilter objects are considered equal
        * iff they belong to the same class and their underlying Filter
        * arrays are deep-equal.
        *
        * @return true iff this ArrayFilter and the passed object
        *         are equivalent
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
        * Obtain the Filter array.
        *
        * @return the Filter array
        */
        virtual ObjectArray::View getFilters() const;

        /**
        * Ensure that the order of underlying filters is preserved by the
        * applyIndex() and evaluateEntry() implementations.
        *
        * @since 12.2.1
        */
        virtual void honorOrder();


    // ----- internal helpers -----------------------------------------------

    protected:
        /**
        * Apply the specified IndexAwareFilter to the specified keySet.
        *
        * @param vFilter      the IndexAwareFilter to apply an index to
        * @param vMapIndexes  the available MapIndex objects keyed by
        *                     the related ValueExtractor; read-only
        * @param hSetKeys     the mutable set of keys that remain to be
        *                     filtered
        *
        * @return a Filter object that can be used to process the remaining
        *         keys, or null if no additional filter processing is
        *         necessary
        */
        virtual Filter::View applyFilter(IndexAwareFilter::View vFilter,
                Map::View vMapIndexes, Set::Handle hSetKeys) const;

        /**
        * Sort all the participating filters according to their effectiveness.
        *
        * @param vMapIndexes  the available MapIndex objects keyed by
        *                     the related ValueExtractor; read-only
        * @param vSetKeys     the set of keys that will be filtered; read-only
        *
        * @since 12.2.1
        */
        virtual void optimizeFilterOrder(Map::View vMapIndexes,
                Set::View vSetKeys) const;


    // ----- inner class: WeightedFilter ------------------------------------

    public:
        /**
        * A thin wrapper around a Filter allowing for sorting the filters
        * according to their effectiveness.
        *
        * @author tb  2009.02.09
        */
        class COH_EXPORT WeightedFilter
            : public class_spec<WeightedFilter,
                    extends<Object>,
                    implements<Comparable> >
            {
            friend class factory<WeightedFilter>;

            // ----- constructors ---------------------------------------

            protected:
                /**
                * Construct the WeightedFilter.
                *
                * @param vFilter  the wrapped filter
                * @param nEffect  the filter's effectiveness
                */
                WeightedFilter(Filter::View vFilter, int32_t nEffect);

            // ----- Comparable interface -------------------------------

            public:
                /**
                * Compares this WeightedFilter with the specified
                * WeightedFilter for order.  Returns a negative integer, zero,
                * or a positive integer as this WeightedFilter's effectiveness
                * is less than, equal to, or greater than the effectiveness of
                * the specified WeightedFilter object.
                *
                * @param v  the Object to be compared
                *
                * @return a negative integer, zero, or a positive integer as
                *         this object is less than, equal to, or greater than
                *         the specified object
                *
                * @throws ClassCastException if the specified object's type
                *         prevents it from being compared to this
                *         WeightedFilter
                */
                int32_t compareTo(Object::View v) const;

            // ----- accessors ------------------------------------------

            public:
                /**
                * Get the wrapped filter.
                *
                * @return the wrapped filter
                */
                Filter::View getFilter() const;

            // ----- data members ---------------------------------------

            protected:
                /**
                * The wrapped filter.
                */
                FinalView<Filter> f_vFilter;

                /**
                * The effectiveness of the wrapped filter.
                */
                int32_t m_nEffect;
            };


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Filter array.
        */
        mutable MemberView<ObjectArray> m_vaFilter;

        /**
        * Flag indicating whether or not the filter order should be preserved.
        *
        * @since Coherence 12.2.1
        */
        mutable bool m_fPreserveOrder;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_EXTRACTOR_FILTER_HPP
