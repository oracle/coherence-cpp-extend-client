/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_DETACH_FINALIZER_HPP
#define COH_DETACH_FINALIZER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/FinalizableBlock.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Finalizer which detaches from an Object upon deletion. The finalizer is
* templated allowing it to work with both const and non-const attachments.
*/
template<class T> class DetachFinalizer
        : public FinalizableBlock::Finalizer
    {
    // ----- constructor ----------------------------------------------------

    public:
        /**
        * Construct a DetachFinalizer to automatically detach from a
        * previously attached Object. The caller must have an unmatched
        * attachment to the Object.
        *
        * @param pDetach  pointer to Object to detach from during
        *                 finalization
        * @param fEscaped if the attachment was escaped
        */
        DetachFinalizer(T* pDetach = NULL, bool fEscaped = true)
                : m_pDetach(pDetach), m_fEscaped(fEscaped)
            {
            }

        /**
        * Destruct the DetachFinalizer, detaching from the Object in the
        * process.
        */
        virtual ~DetachFinalizer()
            {
            T* pDetach = m_pDetach;
            if (NULL != pDetach)
                {
                pDetach->_detach(m_fEscaped);
                }
            }

    // ----- DetachFinalizer interface --------------------------------------

    public:
        /**
        * Set the Object which the finalizer will detach from.  If there was
        * already an Object associated with this finalizer, it will be
        * detached as part of this call.
        *
        * @param pDetach  the Object to detach from upon destruction
        * @param fEscaped if the attachment was escaped
        *
        * @return a pointer to this DetachFinalizer
        */
        DetachFinalizer* set(T* pDetach, bool fEscaped = true)
            {
            T*   pOld        = m_pDetach;
            bool fEscapedOld = m_fEscaped;

            m_pDetach  = pDetach;
            m_fEscaped = fEscaped;

            if (NULL != pOld)
                {
                pOld->_detach(fEscapedOld);
                }
            return this;
            }

    // ----- data members ---------------------------------------------------

    private:
        /**
        * Pointer to Object to detach from.
        */
        T* m_pDetach;

        /**
        * The detach escape type.
        */
        bool m_fEscaped;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_DETACH_FINALIZER_HPP
