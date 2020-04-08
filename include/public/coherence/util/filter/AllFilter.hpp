/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ALL_FILTER_HPP
#define COH_ALL_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"
#include "coherence/util/filter/ArrayFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which returns the logical "and" of a filter array.
*
* @author djl  2008.03.23
*/
class COH_EXPORT AllFilter
    : public class_spec<AllFilter,
        extends<ArrayFilter> >
    {
    friend class factory<AllFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        AllFilter();

        /**
        * Construct an "all" filter. The result is defined as:
        *
        * vaFilter[0] &amp;&amp; vaFilter[1] ... &amp;&amp; vaFilter[n]
        *
        * @param vaFilter  an array of filters
        */
        AllFilter(ObjectArray::View vaFilter);

    private:
        /**
        * Blocked copy constructor.
        */
        AllFilter(const AllFilter&);


    // ----- EntryFilter interface ------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluateEntry(Map::Entry::View vEntry) const;


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
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
    };

COH_CLOSE_NAMESPACE3

#endif // COH_ALL_FILTER_HPP
