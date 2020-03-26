/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_KEY_ASSOCIATED_FILTER_HPP
#define COH_KEY_ASSOCIATED_FILTER_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/util/Filter.hpp"

COH_OPEN_NAMESPACE3(coherence,util,filter)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;


/**
* Filter which limits the scope of another filter according to the key
* association information.
* <p/>
* This filter is intended to be used to optimize queries for partitioned
* caches that utilize any of the key association algorithms (by implementing
* either coherence::net::partition::KeyAssociator or
* coherence::net::cache::KeyAssociation) to ensure placement of
* all associated entries in the same distributed cache partition (and
* therefore in the same storage-enabled cluster node). Using the
* KeyAssociatedFilter will instruct the distributed cache to apply the
* wrapped filter only to the entries stored at the cache service node that
* owns the specified host key.
*
* <b>Note 1:</b> This filter must be the outermost filter and cannot be used
* as a part of any composite filter (AndFilter, OrFilter, etc.)
*
* For example, consider two classes called <i>Parent</i> and <i>Child</i>
* that are stored in separate caches using <i>ParentKey</i> and
* <i>ChildKey</i> objects respectively. The Parent and Child classes have a
* <i>getId</i> method that returns a Long value that uniquely identifies the
* object. Similarly, the ParentKey and ChildKey classes have a <i>getId</i>
* method that uniquely identifies the corresponding cached object.
* Futhermore, the Child and ChildKey classes include a <i>getParentId</i>
* method that returns the Long identifier of the Parent object.
*
* There are two ways to ensure that Child objects are collected with their
* Parent objects (in the same storage-enabled cluster node).
* <ol>
* <li>Make the ChildKey class implement
* coherence::net::cache::KeyAssociation as follows:
* <pre>
* virtual Object::View getAssociatedKey() const
*     {
*     return getParentId();
*     }</pre>
* and the ParentKey class implement
* coherence::net::cache::KeyAssociation as follows:
* <pre>
* public: virtual Object::View getAssociatedKey()
*     {
*     return getId();
*     }</pre>
* Note: a Java version of these classes must also exist and contain the same
* logic</li>
* <li>Implement a custom com.tangosol.coherence.net.partition.KeyAssociator
* in Java which will be processed on the cache servers.
* </li>
* </ol>
* The first approach requires a trivial change to the ChildKey and ParentKey
* classes, whereas the second requires a new class and a configuration
* change, but no changes to existing classes.
* <p/>
* Now, to retrieve all the Child objects of a given Parent using an optimized
* query you would do the following:
* <pre>
* ParentKey::Handle parentKey = ParentKey::create(...);
* Integer64::View   parentId  = parentKey->getId();
*
* // this Filter will be applied to all Child objects in order to fetch those
* // for which getParentId() returns the specified Parent identifier
* Filter::View filterEq = EqualsFilter::create(ReflectionExtractor::create(
*       "getParentId"), parentId);
*
* // this Filter will direct the query to the cluster node that currently
* // owns the Parent object with the given identifier
* Filter::View filterAsc =  KeyAssociatedFilter::create(filterEq, parentId);
*
* // run the optimized query to get the ChildKey objects
* Set::View setChildKeys = cacheChildren->keySet(filterAsc);
*
* // get all the Child objects at once
* Set::Handle setChildren = cacheChildren->getAll(setChildKeys);
* </pre>
* To remove the Child objects you would then do the following:
* <pre>
* cacheChildren->keySet()->removeAll(setChildKeys);</pre>
*
* @author djl  2008.05.19
*/
class COH_EXPORT KeyAssociatedFilter
    : public class_spec<KeyAssociatedFilter,
        extends<Object>,
        implements<PortableObject, Filter> >
    {
    friend class factory<KeyAssociatedFilter>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor (necessary for the PortableObject interface).
        */
        KeyAssociatedFilter();

        /**
        * Construct a key associated filter.
        *
        * @param vFilter    the underlying (wrapped) filter
        * @param vHostKey   the host key that serves as an associated key for
        *                   all keys that the wrapped filter will be applied
        *                   to
        */
        KeyAssociatedFilter(Filter::View vFilter, Object::View vHostKey);


    // ----- Filter interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool evaluate(Object::View v) const;


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
        * Obtain the wrapped Filter.
        *
        * @return the wrapped filter object
        */
       virtual Filter::View getFilter() const;

        /**
        * Obtain the host key that serves as an associated key for all keys
        * that the wrapped filter will be applied to.
        *
        * @return the host key
        */
        virtual Object::View getHostKey() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The underlying filter.
        */
        FinalView<Filter> f_vFilter;

        /**
        * The association host key.
        */
        FinalView<Object> f_vHostKey;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_KEY_ASSOCIATED_FILTER_HPP
