/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_KEY_FILTER_HPP
#define COH_KEY_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/util/Map.hpp"
#include "coherence/util/Set.hpp"
#include "coherence/util/filter/EntryFilter.hpp"
#include "coherence/util/filter/IndexAwareFilter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)


/**
* EntryFilter which checks whether an entry key belongs to a set.
*
* <b>Note: this filter is not serializable and intended to be used only
* internally by other composite filters to reduce the remaining key set.</b>
*
* As of Coherence 3.2, an equivalent functionality could be achieved using the
* InFilter as follows:
* <pre>
*   new InFilter(new KeyExtractor(IdentityExtractor.INSTANCE), setKeys);
* </pre>
*
* @author djl  2008.04.14
*/
class COH_EXPORT KeyFilter
    : public class_spec<KeyFilter,
        extends<Object>,
        implements<IndexAwareFilter> >
    {
    friend class factory<KeyFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a key filter.
        *
        * @param vSetKeys  the keys that this filter will evaluate to true
        */
        KeyFilter(Set::View vSetKeys);


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


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data member accessors ------------------------------------------

    public:
        /**
        * Get the set of keys that are evaluated to true by this filter.
        *
        * @return the set of keys
        */
        virtual Set::View getKeys() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The keys that are evaluated to true by this filter.
        */
        FinalView<Set> f_vSetKeys;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_KEY_FILTER_HPP
