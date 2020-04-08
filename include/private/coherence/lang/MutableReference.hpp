/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MUTABLE_REFERENCE_HPP
#define COH_MUTABLE_REFERENCE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/Reference.hpp"
#include "coherence/lang/TypedHandle.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* MutableReference is an implementation of the Reference interface which
* supports mutating the reference.
*
* @author mf  2008.04.10
*/
class COH_EXPORT MutableReference
    : public class_spec<MutableReference,
        extends<Object>,
        implements<Reference> >
    {
    friend class factory<MutableReference>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new MutableReferance.
        *
        * @param ofReferant  the object to reference
        */
        MutableReference(Object::Holder ohReferant);

    // ----- Reference interface --------------------------------------------

    public:
        /**
        * Set the object to reference.
        */
        virtual void set(Object::Holder ohReferant);

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get() const;

        /**
        * {@inheritDoc}
        */
        virtual Object::Holder get();

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The referenced object.
        */
        MemberHolder<Object> m_ohReferant;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_MUTABLE_REFERENCE_HPP
