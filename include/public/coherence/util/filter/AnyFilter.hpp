/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_ANY_FILTER_HPP
#define COH_ANY_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"
#include "coherence/util/filter/ArrayFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which returns the logical "or" of a filter array.
*
* @author djl  2008.03.23
*/
class COH_EXPORT AnyFilter
    : public class_spec<AnyFilter,
        extends<ArrayFilter> >
    {
    friend class factory<AnyFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        AnyFilter();

        /**
        * Construct an "any" filter. The result is defined as:
        *   vaFilter[0] || vaFilter[1] ... || vaFilter[n]
        *
        * @param vaFilter   an array of filters
        */
        AnyFilter(ObjectArray::View vaFilter);

    private:
        /**
        * Blocked copy constructor.
        */
        AnyFilter(const AnyFilter&);


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

#endif // COH_ANY_FILTER_HPP
