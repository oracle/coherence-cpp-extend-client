/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SYNCHRONIZED_BLOCK_HPP
#define COH_SYNCHRONIZED_BLOCK_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/IllegalStateException.hpp"
#include "coherence/lang/Object.hpp"

#include <sstream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* The SynchronizedBlock class allows for easy creation of synchronized code
* blocks based on an Object's monitor. The SynchronizedBlock object will
* ensure that the monitor is entered and exited as part of starting and
* ending the code block.
*
* Example usage:
* @code
* // outside of sync block
*     {
*     SynchronizedBlock sync(vObject);
*     // monitor entered
*     // critical section goes here
*     // ...
*     } // monitor exited
* // outside of sync block
* @endcode
*
* A more friendly form is to use the COH_SYNCHRONIZED macro. Example usage:
*
* @code
* // outside of sync block
* COH_SYNCHRONIZED(vLock) // monitor entered
*   {
*   // critical section goes here
*   // ...
*   // ...
*   } // monitor exited
* // outside of sync block
* @endcode
*
* @author mf 2007.12.09
*/
class COH_EXPORT SynchronizedBlock
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a SynchronizedBlock object.
        *
        * This will automatically enter the monitor.
        */
        SynchronizedBlock(Object::View v)
            : m_cpObject(get_pointer(v)), m_vObject(v)
            {
            v->_enterMonitor();
            }

        /**
        * Construct a SynchronizedBlock object from a raw object pointer.
        *
        * This will automatically enter the monitor.
        *
        * The caller must ensure that the referenced Object is kept alive for
        * the lifetime of the SynchronizedBlock.  This method is generally
        * used for synchronizing on "this", which implicitly ensures the
        * lifetime guarantee.
        */
        SynchronizedBlock(const Object* p)
            : m_cpObject(p), m_vObject()
            {
            if (p == NULL)
                {
                coh_throw_npe(typeid(Object));
                }
            p->_enterMonitor();
            }

        /**
        * Copy constructor for COH_SYNCHRONIZED macro.
        */
        SynchronizedBlock(const SynchronizedBlock& that)
            : m_cpObject(that.m_cpObject), m_vObject(that.m_vObject)
            {
            that.m_cpObject = NULL;
            that.m_vObject  = NULL;
            }

        /**
        * Destroy a SynchronizedBlock object.
        *
        * This will automatically exit the monitor.
        */
        ~SynchronizedBlock()
            {
            const Object* cp = m_cpObject;
            if (NULL != cp)
                {
                m_cpObject = NULL;
                cp->_exitMonitor();
                m_vObject  = NULL;
                }
            }


    // ----- operators ------------------------------------------------------

    public:
        /*
        * Boolean conversion for use in COH_SYNCHRONIZED macro.
        *
        * @return false if synchronization is held, true otherwise
        */
        operator bool() const
            {
            return m_cpObject == NULL;
            }

    private:
        /**
        * Blocked assignment operator.
        */
        const SynchronizedBlock& operator=(const SynchronizedBlock&);

        /**
        * Blocked dynamic allocation.
        */
        static void* operator new(size_t);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Raw pointer to Object used to protect the synchronized block.
        */
        mutable const Object* m_cpObject;

        /**
        * Object used to protect the synchronized block.
        */
        mutable Object::View m_vObject; // on stack
    };

COH_CLOSE_NAMESPACE2


/**
* Macro for making more readable synchronized code blocks See the
* documentation of SynchronizedBlock for a usage example.
*
* @see coherence::lang::SynchronizedBlock
*/
#define COH_SYNCHRONIZED(V) \
    if (coherence::lang::SynchronizedBlock COH_UNIQUE_IDENTIFIER(_coh_sync_) \
        = coherence::lang::SynchronizedBlock(V)) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

#endif // COH_SYNCHRONIZED_BLOCK_HPP
