/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_GENERIC_SUBJECT_HPP
#define COH_GENERIC_SUBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Set.hpp"



COH_OPEN_NAMESPACE3(coherence,security,auth)

using coherence::util::Set;

/**
* A generic implementation of the subject interface.
*
* @author mf 2008.08.22
*/
class COH_EXPORT GenericSubject
    : public cloneable_spec<GenericSubject,
        extends<Object>,
        implements<Subject> >
    {
    friend class factory<GenericSubject>;

    // ----- constructors ---------------------------------------------------

    private:
        /**
        * Construct a Subject from a Set of Principals.
        *
        * @param vSet  the principal set to copy
        */
        GenericSubject(Set::View vSetPrincipals);

        /**
        * Construct a Subject containing a single GenericPrincipal.
        *
        * @param vsName  the name for the GenericPrincipal
        */
        GenericSubject(String::View vsName);

        /**
        * Copy constructor.
        */
        GenericSubject(const GenericSubject& that);


    // ----- Subject interface ----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual Set::View getPrincipals() const;


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
        * The Principal set.
        */
        FinalView<Set> f_vSetPrincipals;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_GENERIC_SUBJECT_HPP
