/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SMART_MEMBER_HPP
#define COH_SMART_MEMBER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Base class for smart data-members of managed Objects.
*
* Smart data-members include additional information regarding their
* life-cycle, including a reference to their enclosing Object (guardian). The
* smart member is initialized with a reference to its guardian, and has the
* same life-cycle as the guardian. Creating a SmartMember which is not a
* data-member of the guardian used during initialization will result in
* unsafe/undefined behavior.
*
* @author mf 2008.01.30
*/
class COH_EXPORT SmartMember
    {
    // ----- nested type: Mutability ----------------------------------------

    public:
        /**
        * The mutability of the SmartMember.
        */
        typedef enum
            {
            inherited         = 0, // matches parent
            forever_mutable   = 1, // forever mutable
            forever_immutable = 2, // no longer mutable; can't go back
            safe_immutable    = 3  // not mutable, and reads don't need
                                   // synchronization
            } Mutability;


    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Construct a new SmartMember
        *
        * @param oGuardian    the data-member's guardian Object.
        * @param nMutability  the mutability of the SmartMember
        */
        SmartMember(const Object& oGuardian, Mutability nMutability = inherited)
            : m_nMutability(nMutability)
            {
            if (oGuardian._isEscaped())
                {
                m_pGuardian = NULL;
                setGuardian(oGuardian);
                }
            else
                {
                m_pGuardian                   = &oGuardian;
                m_pNext                       = oGuardian.m_pSmartMemberStack;
                oGuardian.m_pSmartMemberStack = this;
                }
            }


        /**
        * Destruct the SmartMember.
        */
        virtual ~SmartMember()
            {
            Object const* pGuardian = m_pGuardian;
            if (NULL != pGuardian && !pGuardian->_isEscaped() &&
                this == pGuardian->m_pSmartMemberStack)
                {
                // we are at the top of the stack
                pGuardian->m_pSmartMemberStack = m_pNext;
                }
            else
                {
                unlinkGuardian();
                }
            }

    protected:
        /**
        * Construct an orphaned SmartMember.
        *
        * The smart-member will not be usable until a guardian is assigned.
        *
        * @see #setGuardian()
        */
        SmartMember();

    private:
        /**
        * Blocked copy constructor.
        */
        SmartMember(const SmartMember&);


    // ----- operators ------------------------------------------------------

    public:
        SmartMember& operator=(const SmartMember& that);


    // ----- SmartMember interface ------------------------------------------

    protected:
        /**
        * @internal
        *
        * Return the guardian Object for this data member.
        *
        * @return the guardian Object for this data member.
        */
        const Object& getGuardian() const
            {
            return *m_pGuardian;
            }

        /**
        * @internal
        *
        * Set the guardian for this member.
        *
        * This method is not thread-safe and should not be called on an
        * escaped object.
        */
        void setGuardian(const Object& oGuardian);

        /**
        * @internal
        *
        * Removes this SmartMember from its guardian.
        */
        void unlinkGuardian();

        /**
         * Return the deep byte size of any managed objects held by this
         * SmartMember.
         */
        virtual size64_t retained() const = 0;

        /**
         * Cross the guardian's read barrier.
         */
        void readBarrier() const
            {
            getGuardian()._readBarrier();
            }

        /**
         * Cross the guardian's write barrier.
         */
        void writeBarrier() const
            {
            getGuardian()._writeBarrier();
            }

    // ----- life cycle events ----------------------------------------------

    protected:
        /**
        * Event called once the guardian Object has finished being constructed.
        * Specifically when the first attachment is made.
        *
        * As with all event methods any derived implementation should
        * include a call to the super class's implementation.
        *
        * The default implementation calls the onInit() method of each of the
        * Object's SmartMembers.
        */
        virtual void onInit()
            {
            }

        /**
        * Event called when the guardian Object becomes only referenced via
        * const pointers (Views). As the SmartMember is a data-member
        * of this Object, the data-member infer that only const methods will
        * be called from now on.
        *
        * Derived implementations of this method should include a delegation
        * call to their super class onConst() method, after they've done
        * their own processing of the event.
        *
        * Not that this method will only be called by the guardian if the
        * SmartMember's mutability state is set to inherited.
        */
        virtual void onConst()
            {
            m_nMutability = forever_immutable;
            }

        /**
        * Event called when the guarding Object's escape state changes.
        *
        * Derived implementations of this method should include a delegation
        * call to their super class onEscape() method, after they've done
        * their own processing of the event.
        *
        * @param fEscaped  true if the object is being escaped, false if the
        *                  object is being captured
        */
        virtual void onEscape(bool /*fEscaped*/) const
            {
            if (m_nMutability == forever_immutable)
                {
                // forever_immutable cannot go back to mutable, and we are single threaded
                // at this point any associated "const" value can be safely
                // read without the need for synchronization
                m_nMutability = safe_immutable;
                }
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The mutability of the SmartMember.
        */
        mutable Mutability m_nMutability;

    private:
        /**
        * The Member's guardian.
        */
        Object const* m_pGuardian;

        /**
        * The next SmartMember in the guardian's SmartMember stack.
        */
        mutable SmartMember* m_pNext;


    // ----- friends --------------------------------------------------------

    friend class Object;

    friend const Object& get_guardian(const SmartMember& member);
    };

/**
* @internal
*
* Return the guardian Object for the smart member.
*
* @return the guardian Object for the smart member.
*/
inline const Object& get_guardian(const SmartMember& member)
    {
    return member.getGuardian();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_SMART_MEMBER_HPP
