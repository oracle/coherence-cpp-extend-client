/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DESCRIBABLE_HPP
#define COH_DESCRIBABLE_HPP

#include "coherence/lang.ns"



COH_OPEN_NAMESPACE2(coherence,util)


/**
* Abstract Object extension that simplifies the implementation of toString()
* for class hierarchies.
*
* Subclasses of Describable should implement the getDescription() method
* by outputing a delimited list of name-value pair(s) representing the class'
* state. Further subclasses should override the getDescription() by first
* delegating to the parent class' implementation and then outputing their own
* state.
*
* For example, consider a class hiearchy consisting of parent class A and
* child class B. Class A should implement getDescription() as follows:
* <pre>
* String::View A::getDescription() const
*     {
*     return COH_TO_STRING(super::getDescription() << "Member1=" << m_hMember1 << ... << ", MemberN=" << m_hMemberN);
*     }
* </pre>
* Class B would then override this method as follows:
* <pre>
* String::View B::getDescription() const
*     {
*     return COH_TO_STRING(super::getDescription() << "MemberN+1=" << m_hMemberN1 << ... );
*     }
* </pre>
*
* @author jh  2008.02.13
*/
class COH_EXPORT Describable
    : public abstract_spec<Describable>
    {
    // ----- Describable interface ------------------------------------------

    public:
        /**
        * Return a human-readable description of the state of this class in
        * the form of a delimited list of name-value pairs.
        *
        * The default implementation returns an empty string
        */
        virtual String::View getDescription() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DESCRIBABLE_HPP
