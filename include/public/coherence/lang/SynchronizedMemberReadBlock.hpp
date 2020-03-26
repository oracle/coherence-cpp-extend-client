/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYNCHRONIZED_MEMBER_READ_BLOCK_HPP
#define COH_SYNCHRONIZED_MEMBER_READ_BLOCK_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/FinalizableBlock.hpp"

#include <iostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* The SynchronizedMemberReadBlock class allows for easy creation of
* synchronized code blocks based on an Object's member level read/write lock.
* The SynchronizedMemberBlock object will ensure that the locks are acquired
* and released as part of starting and ending the code block.
*
* Member read/write locks are not a general purpose feature, and should not
* be used to protect blocking calls. They are intended to protect reads and
* writes to data member primitives, and other short non-blocking code
* blocks.
*
* Example usage:
* @code
* // outside of sync block
*     {
*     SynchronizedMemberReadBlock syncRead(self()); // read lock acquired
*     // critical section goes here
*     // ...
*     } // read lock released
* // outside of sync block
* @endcode
*
* A more friendly form is to use the COH_SYNCHRONIZED_MEMBER_READ
* macros. Example usage:
*
* @code
* // outside of sync block
* COH_SYNCHRONIZED_MEMBER_READ // read lock acquired
*   {
*   // critical section goes here
*   // ...
*   // ...
*   } // read lock released
* // outside of sync block
* @endcode
*
* The SynchronizedMemberReadBlock class relies on its creator to ensure that
* the associated Object outlives the scope of the block. The helper macro
* ensures this by only allowing you to create a block for the encompassing
* Object, i.e. "this". If the blocks are manually created then the caller
* must ensure that the associated Object outlives the block.
* '
* Note: This class derives from FinalizableBlock, allowing custom finalizers
* to be registered.  The finalizers will execute after the lock has been
* released.
*
* @author mf 2008.01.29
*/
class COH_EXPORT SynchronizedMemberReadBlock
        : public FinalizableBlock
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a synchronized SynchronizedMemberReadBlock, acquiring the
        * Object's member read lock.
        *
        * @param o          the Object to lock
        * @param pDelegate  SynchronizedMemberReadBlock to delegate to, or
        *                   NULL for no delegate
        *
        * @throws IllegalArgumentException if pSyncDelegate is non-NULL and
        *         references a different Object.
        */
        SynchronizedMemberReadBlock(const Object& o,
                SynchronizedMemberReadBlock* pDelegate = NULL)
            : FinalizableBlock(), m_cpObject(NULL)
            {
            initialize(o, pDelegate);
            if (isTerminal())
                {
                o._acquireMemberReadLock();
                }
            m_cpObject = &o;
            }

        /**
        * Destroy a SynchronizedMemberReadBlock.
        *
        * This is a no-op for a delegating block, otherwise the read lock is
        * released.
        */
        ~SynchronizedMemberReadBlock()
            {
            const Object* cpObject = m_cpObject;
            if (NULL != cpObject && isTerminal())
                {
                try
                    {
                    cpObject->_releaseMemberReadLock();
                    }
                catch (const std::exception& e)
                    {
                    std::cerr << "Error releasing MemberReadLock: " <<
                        e.what() << std::endl;
                    }
                }
            }

    protected:
        /**
        * Construct an uninitialized SynchronizedMemberReadBlock.
        */
        SynchronizedMemberReadBlock()
            : FinalizableBlock(), m_cpObject(NULL)
            {
            }


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Validate that the lock is held. The boolean conversion is utilized
        * by the COH_SYNCHRONIZED_MEMBER_READ macro.
        *
        * @throw IllegalStateException if the lock is not held.
        *
        * @return false always
        */
        operator bool() const
            {
            if (NULL == m_cpObject)
                {
                coh_throw_illegal_state("lock not held");
                }
            return FinalizableBlock::operator bool();
            }

    // -------- SynchronizedMemberReadBlock interface -----------------------

    public:
        /**
        * Get the value of the specified member, without obtaining
        * additional synchronization.
        *
        * This helper function is only supported on "members" which
        * supply a custom two-parameter "get" method utilizing the
        * SynchronizedMemberBlock::SynchronizedMemberReadBlock facility.
        *
        * @param member  the member whose value to get
        *
        * @return the member's value
        */
        template<class M> typename M::ConstGetType getMember(const M& member)
            {
            return member.get(this);
            }

        /**
        * Get the value of the specified member, without obtaining
        * additional synchronization.
        *
        * This helper function is only supported on "members" which
        * supply a custom parameterized "get" method utilizing the
        * SynchronizedMemberBlock::SynchronizedMemberReadBlock facility.
        *
        * @param member  the member whose value to get
        *
        * @return the member's value
        */
        template<class M> typename M::GetType getMember(M& member)
            {
            return member.get(this);
            }

    protected:
        /**
        * Initialize a synchronized SynchronizedMemberReadBlock.
        *
        * @param o          the Object associated with the block
        * @param pDelegate  SynchronizedMemberReadBlock to delegate to, or
        *                   NULL for no delegate
        *
        * @throws IllegalArgumentException if pSyncDelegate is non-NULL and
        *         references a different Object.
        */
        void initialize(const Object& o,
                SynchronizedMemberReadBlock* pDelegate = NULL)
            {
            FinalizableBlock::initialize(pDelegate);

            if (!isTerminal() && &o != pDelegate->m_cpObject)
                {
                coh_throw_illegal_argument("Object differs from delegate");
                }
            }


    // ----- nested class: Guard --------------------------------------------

    public:
        /**
        * Simple read lock structure for use in inlining.
        */
        class COH_EXPORT Guard
            {
            public:
                Guard(const Object& o)
                    : m_o(o)
                    {
                    acquireMemberReadLock(o);
                    }

                ~Guard()
                    {
                    releaseMemberReadLock(m_o);
                    }

            private:
                const Object& m_o;
            };


    // ----- helper methods -------------------------------------------------

    protected:
        /*
        * Acquire an Object's read lock
        */
        static void acquireMemberReadLock(const Object& o)
            {
            o._acquireMemberReadLock();
            }

        /*
        * Release an Object's read lock
        */
        static void releaseMemberReadLock(const Object& o)
            {
            o._releaseMemberReadLock();
            }


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Object on which the lock is applied.
        */
        const Object* m_cpObject;
    };

COH_CLOSE_NAMESPACE2


/**
* Macro for making more readable synchronized member read code blocks See the
* documentation of SynchronizedMemberReadBlock for a usage example.
*
* @see coherence::lang::SynchronizedMemberReadBlock
*/
#define COH_SYNCHRONIZED_MEMBER_READ \
    if (coherence::lang::SynchronizedMemberReadBlock coh_synchronized_member_read \
        = coherence::lang::SynchronizedMemberReadBlock(Object::self())) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

#endif // COH_SYNCHRONIZED_MEMBER_READ_BLOCK_HPP
