/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYNCHRONIZED_MEMBER_WRITE_BLOCK_HPP
#define COH_SYNCHRONIZED_MEMBER_WRITE_BLOCK_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/SynchronizedMemberReadBlock.hpp"

#include <iostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* The SynchronizedMemberBlock class allows for easy creation of synchronized
* code blocks based on an Object's member level read/write lock. The
* SynchronizedMemberBlock object will ensure that the locks are acquired and
* released as part of starting and ending the code block.
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
*     SynchronizedMemberWriteBlock syncWrite(self()); // write lock
*     acquired // critical section goes here
*     // ...
*     } // write lock released
* // outside of sync block
* @endcode
*
* A more friendly form is to use the COH_SYNCHRONIZED_MEMBER_WRITE
* macros. Example usage:
*
* @code
* // outside of sync block
* COH_SYNCHRONIZED_MEMBER_WRITE // write lock acquired
*   {
*   // critical section goes here
*   // ...
*   // ...
*   } // write lock released
* // outside of sync block
* @endcode
*
* The SynchronizedMemberWriteBlock class relies on its creator to ensure that
* the associated Object outlives the scope of the block. The helper macro
* ensures this by only allowing you to create a block for the encompassing
* Object, i.e. "this". If the blocks are manually created then the caller
* must ensure that the associated Object outlives the block.
*
* Note: This class indirectly derives from FinalizableBlock, allowing custom
* finalizers to be registered. The finalizers will execute after the lock has
* been released.
*
* @author mf 2008.01.29
*/
class COH_EXPORT SynchronizedMemberWriteBlock
        : public SynchronizedMemberReadBlock
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a synchronized SynchronizedMemberWriteBlock, acquiring the
        * Object's member write lock.
        *
        * @param o          the Object to lock
        * @param pDelegate  SynchronizedMemberWriteBlock to delegate to, or
        *                   NULL for no delegate
        *
        * @throws IllegalArgumentException if pSyncDelegate is non-NULL and
        *         references a different Object.
        */
        SynchronizedMemberWriteBlock(const Object& o,
                SynchronizedMemberWriteBlock* pDelegate = NULL)
            : SynchronizedMemberReadBlock()
            {
            initialize(o, pDelegate);
            if (isTerminal())
                {
                o._acquireMemberWriteLock();
                }
            m_cpObject = &o;
            }

        /**
        * Destroy a SynchronizedMemberWriteBlock.
        *
        * This is a no-op for a delegating block, otherwise the write lock is
        * released.
        */
        ~SynchronizedMemberWriteBlock()
            {
            const Object* cpObject = m_cpObject;
            if (NULL != cpObject && isTerminal())
                {
                m_cpObject = NULL; // record that the lock has been released
                try
                    {
                    cpObject->_releaseMemberWriteLock();
                    }
                catch (const std::exception& e)
                    {
                    std::cerr << "Error releasing MemberWriteLock: " <<
                        e.what() << std::endl;
                    }
                }
            }


    // ----- SynchronizedMemberWriteBlock interface -------------------------

    public:
        /**
        * Set the specified member to the specified value, without
        * obtaining additional synchronization.
        *
        * This helper function is only supported on "members" which
        * supply a custom two-parameter "set" method utilizing the
        * SynchronizedMemberWriteBlock facility.
        *
        * @param member  the member to set
        * @param value   the value to set the member to
        */
        template<class M, class V> void setMember(M& member, V value)
            {
            member.set(value, this);
            }

        /**
        * Set the specified member to the specified value so long as
        * the current value is equal to the assumed value.
        *
        * This helper function is only supported on "members" which
        * supply a custom parameterized "set" and "get" methods utilizing the
        * SynchronizedMemberWriteBlock facility.
        *
        * @param member        the member to set
        * @param valueAssumed  the assumed current value of the member
        * @param valueNew      the value to set the member to
        *
        * @return the actual value prior to the update attempt
        */
        template<class M, class V> typename M::GetType updateMember(
                    M& member, V valueAssumed, V valueNew)
            {
            V valueActual = member.get(this);
            if (valueAssumed == valueActual)
                {
                member.set(valueNew, this);
                }
            return valueActual;
            }

        /**
        * Set the specified member to the specified value and return the
        * previous value.
        *
        * This helper function is only supported on "members" which
        * supply a custom parameterized "set" and "get" methods utilizing the
        * SynchronizedMemberWriteBlock facility.
        *
        * @param member        the member to set
        * @param valueNew      the value to set the member to
        *
        * @return the prior value
        */
        template<class M, class V> typename M::GetType exchangeMember(
                    M& member, V valueNew)
            {
            V valuePre = member.get(this);
            member.set(valueNew, this);
            return valuePre;
            }


    // ----- nested class: Guard --------------------------------------------

    public:
        /**
        * Simple write lock structure for use in inlining.
        */
        class COH_EXPORT Guard
            {
            public:
                Guard(const Object& o)
                    : m_o(o)
                    {
                    acquireMemberWriteLock(o);
                    }

                ~Guard()
                    {
                    releaseMemberWriteLock(m_o);
                    }

            private:
                const Object& m_o;
            };


    // ----- helper methods -------------------------------------------------

    protected:
        /*
        * Acquire an Object's write lock
        */
        static void acquireMemberWriteLock(const Object& o)
            {
            o._acquireMemberWriteLock();
            }

        /*
        * Release an Object's write lock
        */
        static void releaseMemberWriteLock(const Object& o)
            {
            o._releaseMemberWriteLock();
            }
    };

COH_CLOSE_NAMESPACE2


/**
* Macro for making more readable synchronized member write code blocks See the
* documentation of SynchronizedMemberWriteBlock for a usage example.
*
* @see coherence::lang::SynchronizedMemberWriteBlock
*/
#define COH_SYNCHRONIZED_MEMBER_WRITE \
    if (coherence::lang::SynchronizedMemberWriteBlock coh_synchronized_member_write \
        = coherence::lang::SynchronizedMemberWriteBlock(Object::self())) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

#endif // COH_SYNCHRONIZED_MEMBER_WRITE_BLOCK_HPP
