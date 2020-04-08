/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GENERIC_PRINCIPAL_HPP
#define COH_GENERIC_PRINCIPAL_HPP

#include "coherence/lang.ns"

#include "coherence/security/Principal.hpp"

COH_OPEN_NAMESPACE2(coherence,security)


/**
* A generic implementation of the Principal interface.
*
* @author mf  2008.08.22
*/
class COH_EXPORT GenericPrincipal
    : public cloneable_spec<GenericPrincipal,
        extends<Object>,
        implements<Principal> >
    {
    friend class factory<GenericPrincipal>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Construct a new GenericPrincipal for the given name.
        *
        * @param vsName  the name of the principal
        */
        GenericPrincipal(String::View vsName);

        /**
        * Copy constructor.
        */
        GenericPrincipal(const GenericPrincipal& that);


    // ----- Principal interface --------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual String::View getName() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual bool equals(Object::View vThat) const;

        /**
        * {@inheritDoc}
        */
        virtual size32_t hashCode() const;

        /**
        * {@inheritDoc}
        */
        virtual bool isImmutable() const;

        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    private:
        /**
        * The principal name.
        */
        FinalView<String> f_vsName;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_GENERIC_PRINCIPAL_HPP
