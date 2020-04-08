/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SIMPLE_MAP_ENTRY_HPP
#define COH_SIMPLE_MAP_ENTRY_HPP

#include "coherence/lang.ns"

#include "coherence/util/MapIndex.hpp"
#include "coherence/util/MapTrigger.hpp"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* A map entry (key-value pair).
*
* The <tt>Map::entrySet</tt> method returns a collection-view of the map,
* whose elements are of this class. The <i>only</i> way to obtain a reference
* to a map entry is from the iterator of this collection-view.  These
* <tt>Map::Entry</tt> objects are valid <i>only</i> for the duration of the
* iteration; more formally, the behavior of a map entry is undefined if the
* backing map has been modified after the entry was returned by the iterator,
* except through the iterator's own <tt>remove</tt> operation, or through the
* <tt>setValue</tt> operation on a map entry returned by the iterator.
*
* @author jh  2008.03.13
*/
class COH_EXPORT SimpleMapEntry
    : public cloneable_spec<SimpleMapEntry,
        extends<Object>,
        implements<MapTrigger::Entry> >
    {
    friend class factory<SimpleMapEntry>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new SimpleMapEntry with the given key and value.
        *
        * @param vKey        the key
        * @param ohValue     the value
        * @param vOrigValue  the original value
        */
        SimpleMapEntry(Object::View vKey, Object::Holder ohValue,
                Object::View vOrigValue = NULL);

        /**
        * Copy constructor.
        */
        SimpleMapEntry(const SimpleMapEntry& that);


    // ----- Map::Entry interface -------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getKey() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder getValue();

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setValue(Object::Holder ohValue);


    // ----- MapTrigger::Entry interface ------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getOriginalValue() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isOriginalPresent() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder setValue(Object::Holder ohValue, bool fSynthetic);

        /**
        * {@inheritDoc}
        */
        virtual void update(ValueUpdater::View vUpdater, Object::Holder ohValue);

        /**
        * {@inheritDoc}
        */
        virtual bool isPresent() const;

        /**
        * {@inheritDoc}
        */
        virtual void remove(bool fSynthetic);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder extract(ValueExtractor::View vExtractor) const;


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


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The key.
        */
        FinalView<Object> f_vKey;

        /**
        * The value. This object reference can change within the life of
        * the Entry.
        */
        MemberHolder<Object> m_ohValue;

        /**
        * The original value. This object reference will not change within the life
        * of the Entry.
        */
        FinalView<Object> f_vOrigValue;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SIMPLE_MAP_ENTRY_HPP
