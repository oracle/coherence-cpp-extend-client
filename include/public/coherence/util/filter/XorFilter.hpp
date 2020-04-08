/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XOR_FILTER_HPP
#define COH_XOR_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"
#include "coherence/util/filter/ArrayFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* Filter which returns the logical exclusive or ("xor") of two other filters.
*
* @author djl  2008.04.14
*/
class COH_EXPORT XorFilter
    : public class_spec<XorFilter,
        extends<ArrayFilter> >
    {
    friend class factory<XorFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        XorFilter();

        /**
        * Construct an XorFilter. The result is defined as:
        *   vFilterLeft ^ vFilterRight
        *
        * @param vFilterLeft   the "left" filter
        * @param vFilterRight  the "right" filter
        */
        XorFilter(Filter::View vFilterLeft, Filter::View vFilterRight);

    private:
        /**
        * Blocked copy constructor.
        */
        XorFilter(const XorFilter&);


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

#endif // COH_XOR_FILTER_HPP
