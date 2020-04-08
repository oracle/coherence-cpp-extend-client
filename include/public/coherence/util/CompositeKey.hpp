/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_COMPOSITE_KEY_HPP
#define COH_COMPOSITE_KEY_HPP

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/net/cache/KeyAssociation.hpp"



COH_OPEN_NAMESPACE2(coherence,util)

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::cache::KeyAssociation;


/**
* Key class that consists of a primary and secondary component. Two instances
* of CompositeKey are considered to be equal iff both the primary and
* secondary components of the two instances are considered equal.
* Additionally, the hash code of a CompositeKey takes into the consideration
* the hash codes of its two components. Finally, the CompositeKey class
* implements KeyAssociation by returning the primary component.
*
* @author wl 2010.09.02
*/
class COH_EXPORT CompositeKey
    : public class_spec<CompositeKey,
        extends<Object>,
        implements<KeyAssociation, PortableObject> >
    {
    friend class factory<CompositeKey>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Default constructor.
        */
        CompositeKey();

        /**
        * Create a new CompositeKey that consists of the given primary and
        * secondary components.
        *
        * @param voPrimary   the primary key component; must not be null. This
        *                    is also the host key returned by the 
        *                    KeyAssociation implementation
        * @param voSecondary the secondary key component; must not be null
        */
        CompositeKey(Object::View voPrimary, Object::View voSecondary);


    // ----- KeyAssociation interface ---------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Object::View getAssociatedKey() const;


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


    // ----- Object methods -------------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View v) const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- accessors ------------------------------------------------------

    public:
        /**
        * Return the primary key component.
        *
        * @return the primary key component
        */
        virtual Object::View getPrimaryKey() const;

        /**
        * Return the secondary key component.
        *
        * @return the secondary key component
        */
        virtual Object::View getSecondaryKey() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The primary key component.
        */
        FinalView<Object> f_voPrimary;

        /**
        * The secondary key component.
        */
        FinalView<Object> f_voSecondary;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_COMPOSITE_KEY_HPP
