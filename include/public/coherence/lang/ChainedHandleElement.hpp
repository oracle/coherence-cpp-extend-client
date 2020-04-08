/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_CHAINED_HANDLE_ELEMENT_HPP
#define COH_CHAINED_HANDLE_ELEMENT_HPP

#include "coherence/lang/compatibility.hpp"

#include <stdlib.h>

COH_OPEN_NAMESPACE2(coherence,lang)

/**
* @internal
*
* The base class for TypedHandles, allowing for reference chaining.  Reference
* chaining is not thread-safe and thus may only be used for handles which are
* not shared across threads.
*
* @author mf  2008.10.20
*/
class ChainedHandleElement
    {
    protected:
        /**
        * Action to take on unlinking
        */
        typedef enum
            {
            action_none   = 0, // no action required, chain still exists
            action_flip   = 1, // unlink last handle from handle->view chain
            action_detach = 2, // unlink final element of chain
            action_error  = 3  // corrupted chain, raise error
            } Action;


    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create an independent element.
        */
        ChainedHandleElement(bool fView)
            : m_fView(fView)
            {
            // new chain
            m_prev = m_next = this;
            }

        /**
        * Create a dependent element, adding it to a chain.
        */
        ChainedHandleElement(const ChainedHandleElement& that, bool fView)
            : m_prev(&that), m_next(that.m_next), m_fView(fView)
            {
            // add self to chain
            m_next->m_prev = m_prev->m_next = this;
            }

        /**
        * Create an optionally dependent element.
        */
        ChainedHandleElement(const ChainedHandleElement* that, bool fView)
            : m_fView(fView)
            {
            if (that)
                {
                // add self to chain
                m_prev = that;
                m_next = that->m_next;
                m_next->m_prev = m_prev->m_next = this;
                }
            else
                {
                // new chain
                m_prev = m_next = this;
                }
            }

        /**
        * No destructor, derived class must perform unlink and take resulting
        * action.
        */
        //~ChainedHandleElement()


    // ----- ChainedHandleElement interface ---------------------------------

    protected:
        /**
        * Unlink this element from the chain.
        *
        * @return the action to be taken
        */
        Action unlink() const
            {
            Action nAction =
                    this == m_prev /* implies && this == m_next */ ? action_detach
                    // COH-9499 - removed m_prev and m_next m_fView pre-checks due to
                    // compiler issues with GCC 4.1
                  : !m_fView && scan()                             ? action_flip
                                                                   : action_none;

            // unlink self from chain
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;

            // empty chain
            m_prev = m_next = this;

            return nAction;
            }

        /**
        * Link this element to a new chain.
        *
        * @param that  the element to link to
        *
        * @return the action to be taken
        */
        Action link(const ChainedHandleElement& that) const
            {
            Action nAction =
                    this == m_prev /* implies && this == m_next */ ? action_detach
                    // COH-9499 - removed m_prev and m_next m_fView pre-checks due to
                    // compiler issues with GCC 4.1
                  : !m_fView && scan()                             ? action_flip
                                                                   : action_none;

            // unlink self from chain
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;

            // link self into new chain
            m_prev = &that;
            m_next = that.m_next;

            m_next->m_prev = that.m_next = this;

            return nAction;
            }

        /**
        * Scan the chain for additional handles.
        *
        * The caller must be a handle, and have found that m_prev != m_next != this
        *
        * @return true if this is the only handle in the chain
        */
        bool scan() const
            {
            // The current element must be a handle, so we can scan the list
            // until we encounter a handle; if it is this element then there
            // are no other handles present. While this is a potentially
            // expensive call it is also rarely occurs on a non-trivial chain.
            // Since you cannot assign a Handle from a View this will only
            // happen if multiple Views are injected in the middle of a handle
            // chain. Overall this minimal rare cost is more than worth it as
            // it allows chaining of Handles and Views, which removes the far
            // more expensive (potentially atomic) reference count increment
            // for the common pattern of passing a Handle to a function which
            // takes a View.
            ChainedHandleElement const* pElm = m_prev;
            if (pElm->m_fView && m_next->m_fView)
                for (pElm = pElm->m_prev; pElm->m_fView; pElm = pElm->m_prev);
            // else; common case handle->this handle
            return pElm == this;
            }


    // ----- blocked methods ------------------------------------------------

    private:
        /**
        * Blocked dynamic allocation.
        */
        static void* operator new(size_t cb);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The previous element in the chain.
        */
        mutable ChainedHandleElement const* m_prev;

        /**
        * The next element in the chain.
        */
        mutable ChainedHandleElement const* m_next;

        /**
        * Flag indicating if the associated handle is a view.
        */
        bool m_fView;


    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class> friend class TypedHandle;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_CHAINED_HANDLE_ELEMENT_HPP
