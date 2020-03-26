/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_MEMBER_HANDLE_HPP
#define COH_MEMBER_HANDLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/SmartMember.hpp"
#include "coherence/lang/SynchronizedMemberReadBlock.hpp"
#include "coherence/lang/SynchronizedMemberWriteBlock.hpp"
#include "coherence/lang/TypedHandle.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)

template<class> class MemberView;


/**
* MemberHandle is a thread-safe handle used by an Object to reference its
* non-const child Objects.
*
* MemberHandles transfer the constness of their guardian Object. When a
* MemberHandle is accessed from within a const method of the enclosing
* "parent" class, it will only provide const access to the Object which it
* references. If the enclosing Object becomes only accessed via views the
* MemberHandle will also automatically, and permanently switch its reference
* type from a handle to a view.
*
* Note: In the rare case that a MemberHandle is declared via the mutable
*       keyword, the MemberHandle must be informed of this fact by setting
*       fMutable to true during construction.
*
* @author mf  2007.07.05
*
* @see MemberView
* @see MemberHolder
*/
template<class T>
class MemberHandle
    : public SmartMember, public ChainedHandleElement
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The type of the values the handle can reference.
        */
        typedef T ValueType;

        /**
        * The Handle type for the referenced Object.
        */
        typedef typename T::Handle ValueHandle;

        /**
        * The View type for thekil referenced Object.
        */
        typedef typename T::View ValueView;

        /**
        * Result type for a non-const get operation.
        */
        typedef ValueHandle GetType;

        /**
        * Result type for a non-const get operation.
        */
        typedef ValueView ConstGetType;


    // -------- constructors ------------------------------------------------

    public:
        /**
        * Construct a new MemberHandle referencing NULL via a handle.
        *
        * @param oGuardian  the object that protects this member
        */
        MemberHandle(const Object& oGuardian)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ false),
              m_po(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            }

        /**
        * Construct a new MemberHandle referencing the specified Object via
        * a handle.
        *
        * @param oGuardian   the object that protects this member
        * @param that        the Object to reference
        */
        MemberHandle(const Object& oGuardian, const ValueHandle& that)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ false),
              m_po(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            set(that);
            }

        /**
        * Construct a new MemberHandle referencing the specified Object via
        * a handle.
        *
        * @param oGuardian  the object that protects this member
        * @param that       the Object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        MemberHandle(const Object& oGuardian, const ValueHandle& that,
                bool fMutable)
            : SmartMember(oGuardian), ChainedHandleElement(/*fView*/ false),
              m_po(NULL)
            {
            if (oGuardian._isEscaped())
                {
                m_prev = m_next = NULL;
                }
            set(that);
            m_nMutability = fMutable ? forever_mutable : safe_immutable;
            }

        /**
        * Destroy the MemberHandle.
        */
        ~MemberHandle()
            {
            try
                {
                const T* cpo = m_po;
                if (cpo && m_fView && m_prev == NULL)
                    {
                    // escaped and const (view based ref); set() does not
                    // handle this case as it can only occur during destruction
                    cpo->_detach(/*fEscaped*/ true);
                    }
                else
                    {
                    // even if cpo == NULL we still may need to call unlink()
                    m_nMutability = inherited;
                    set(NULL);
                    }
                }
            catch (const std::exception& e)
                {
                // Exception::View is not a known type within this file
                std::cerr << "Error during ~MemberHandle: " << e.what() << std::endl;
                return; // can't re-throw from within destructor
                }
            }

    private:
        /**
        * Blocked copy constructor.
        */
        MemberHandle(const MemberHandle&);


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Assign the MemberHandle to reference another object.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberHandle
        */
        MemberHandle& operator=(const ValueHandle& that)
            {
            set(that);
            return *this;
            }

        /**
        * Assign this MemberHandle to reference the same Object as the
        * specified MemberHandle.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberHandle
        */
        MemberHandle& operator=(MemberHandle& that)
            {
            operator=((ValueHandle) that); // assign from snapshot
            return *this;
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        operator ValueView() const
            {
            return get();
            }

        /**
        * Return a Handle to the referenced Object.
        *
        * @return a Handle to the referenced Object
        */
        operator ValueHandle()
            {
            return get();
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        template<class PT>
        operator TypedHandle<const PT>() const
            {
            return get();
            }

        /**
        * Return a Handle to the referenced Object.
        *
        * @return a Handle to the referenced Object
        */
        template<class PT>
        operator TypedHandle<PT>()
            {
            return get();
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        *
        */
        template<class PT>
        operator TypedHolder<PT>() const
            {
            return get();
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        */
        template<class PT>
        operator TypedHolder<PT>()
            {
            return get();
            }

        /**
        * Dereference the MemberHandle.
        *
        * @return a const pointer to the referenced Object
        */
        ValueView operator->() const
            {
            return get();
            }

        /**
        * Dereference the MemberHandle.
        *
        * @return a const pointer to the referenced Object
        */
        ValueHandle operator->()
            {
            return get();
            }

        /**
        * Dereference this handle, returning <tt>T&</tt>.
        *
        * @return a raw <tt>T&</tt> reference to the referenced Object
        *
        * @throws NullPointerException if the this handle is @c NULL
        */
        const T& operator*() const
            {
            return *get();
            }

        /**
        * Dereference this handle, returning <tt>T&</tt>.
        *
        * @return a raw <tt>T&</tt> reference to the referenced Object
        *
        * @throws NullPointerException if the this handle is @c NULL
        */
        T& operator*()
            {
            return *get();
            }

    private:
        /**
        * Blocked assignment operator.
        */
        MemberHandle<T>& operator=(const MemberHandle<T>&);


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Set the MemberHandle to reference the same Object as the supplied
        * Handle.
        *
        * @param that   the handle to copy
        * @param pSync  optional external SyncornizedMemberBlock to use
        *               to avoid internal synchronization.
        */
        void set(const TypedHandle<T>& that,
                SynchronizedMemberWriteBlock* pSync = NULL)
            {
            if (m_prev == NULL)
                {
                setEscaped(that, pSync);
                }
            else if (m_nMutability >= forever_immutable)
                {
                coh_throw_illegal_state("attempt to set const MemberHandle");
                }
            else
                {
                performAction(link(that));
                m_po = get_pointer(that);
                }
            }

        /**
        * Set the MemberHandle to reference the same Object as the supplied
        * Handle.
        *
        * @param that   the handle to copy
        * @param pSync  optional external SyncornizedMemberBlock to use
        *               to avoid internal synchronization.
        */
        void setEscaped(const TypedHandle<T>& that,
                SynchronizedMemberWriteBlock* pSync = NULL)
            {
            const Object& oGuardian = getGuardian();
            T*            po        = get_pointer(that);
            T*            pDetach   = NULL;
            Object*       pAttach   = po == NULL ? NULL : po->_attach(/*fEscaped*/ true);

            if (pSync == NULL)
                {
                // sync block
                SynchronizedMemberWriteBlock::Guard guard(oGuardian);
                if (m_nMutability >= forever_immutable)
                    {
                    coh_throw_illegal_state("attempt to set const MemberHandle");
                    }
                pDetach = m_po;
                m_po    = NULL == pAttach ? NULL : po;
                }
            else
                {
                // sync block
                SynchronizedMemberWriteBlock syncWrite(oGuardian, pSync);
                if (m_nMutability >= forever_immutable)
                    {
                    coh_throw_illegal_state("attempt to set const MemberHandle");
                    }
                pDetach = m_po;
                m_po    = NULL == pAttach ? NULL : po;
                }

            if (pDetach)
                {
        	pDetach->_detach(/*fEscaped*/ true);
                }
            }

        /**
        * Return a View to the referenced Object.
        *
        * @param pSync  optional external SyncornizedMemberReadBlock to use
        *               to avoid internal synchronization.
        *
        * @return a View to the referenced Object
        */
        ValueView get(SynchronizedMemberReadBlock* pSync = NULL) const
            {
            if (m_prev == NULL)
                {
                return getEscaped(pSync);
                }
            else
                {
                return TypedHandle<const T>(m_po, *this);
                }
            }

        /**
        * Return a View to the referenced Object.
        *
        * @param pSync  optional external SyncornizedMemberReadBlock to use
        *               to avoid internal synchronization.
        *
        * @return a View to the referenced Object
        */
        ValueView getEscaped(SynchronizedMemberReadBlock* pSync = NULL) const
            {
            if (pSync != NULL)
                {
                SynchronizedMemberReadBlock syncRead(getGuardian(), pSync);
                // return of escaped must occur within scope of syncRead
                return ValueView((const T*) m_po);
                }
            else if (m_nMutability == safe_immutable)
                {
                // safe_immutable, synchronization not required; also implies view
                return ValueView((const T*) m_po);
                }
            else
                {
                SynchronizedMemberReadBlock::Guard guard(getGuardian());
                // return of escaped must occur within scope of guard
                return ValueView((const T*) m_po);
                }
            }

        /**
        * Return a Handle to the referenced Object.
        *
        * @param pSync  optional external SyncornizedMemberReadBlock to use
        *               to avoid internal synchronization.
        *
        * @return a Handle to the referenced Object
        */
        ValueHandle get(SynchronizedMemberReadBlock* pSync = NULL)
            {
            if (m_prev == NULL)
                {
                return getEscaped(pSync);
                }
            else
              {
                return TypedHandle<T>(m_po, *this);
              }
            }

        /**
        * Return a Handle to the referenced Object.
        *
        * @param pSync  optional external SyncornizedMemberReadBlock to use
        *               to avoid internal synchronization.
        *
        * @return a Handle to the referenced Object
        */
        ValueHandle getEscaped(SynchronizedMemberReadBlock* pSync = NULL)
            {
            const Object& oGuardian = getGuardian();
            if (pSync == NULL)
                {
                SynchronizedMemberReadBlock::Guard guard(oGuardian);
                if (m_fView)
                    {
                    coh_throw_illegal_state(
                        "attempt to access handle from const MemberHandle");
                    }
                return ValueHandle(m_po); // must occur within scope of syncRead
                }
            else
                {
                SynchronizedMemberReadBlock syncRead(oGuardian, pSync);
                if (m_fView)
                    {
                    coh_throw_illegal_state(
                        "attempt to access handle from const MemberHandle");
                    }
                return ValueHandle(m_po); // must occur within scope of syncRead
                }
            }

        /**
        * Perform the specified action.
        *
        * @param nAction  the action to perform
        */
        void performAction(Action nAction) const
            {
            T* po = m_po;
            switch (nAction)
                {
                case action_error:
                    coh_throw_illegal_state("corrupted ChainedHandleElement");
                case action_flip:
                    if (NULL != po) ((const T*) po)->_attach(/*fEscaped*/ false);
                    // fall through
                case action_detach:
                    if (NULL != po)
                        {
                        if (m_fView)
                            {
                            ((const T*) po)->_detach(/*fEscaped*/ false);
                            }
                        else
                            {
                            po->_detach(/*fEscaped*/ false);
                            }
                        }
                    // fall through
                case action_none:
                default:
                    break;
                }
            }

    // ----- SmartMember interface ------------------------------------------

    protected:
        /**
        * {@inheritDoc}
        *
        * The MemberHandle is automatically flipped when the guardian Object
        * becomes only referenced via consts.
        */
        virtual void onConst()
            {
            bool fEscaped  = m_prev == NULL;
            T*   po        = NULL;

            if (fEscaped)
                {
                SynchronizedMemberWriteBlock::Guard guard(getGuardian());
                po = m_po;
                }
            else
                {
                po = m_po;
                }

            if (NULL != po)
                {
                // re-attach as const, then detach the old reference
                if (fEscaped)
                    {
                    ((const T*) po)->_attach(/*fEscaped*/ true);
                    po->_detach(/*fEscaped*/ true);
                    }
                else if (scan())
                    {
                    performAction(action_flip);
                    }
                }

            m_fView = true;
            SmartMember::onConst();
            }

        /**
        * {@inheritDoc}
        */
        virtual void onEscape(bool fEscaped) const
            {
            T* po = m_po;
            if (po != NULL)
                {
                if (m_fView)
                    {
                    const T* cpo = po;
                    cpo->_attach(fEscaped);  // new attach
                    }
                else
                    {
                    po->_attach(fEscaped);  // new attach
                    }
                }

            if (fEscaped)
                {
                performAction(unlink());
                m_prev = m_next = NULL;
                }
            else
                {
                if (po)
                    {
                    if (m_fView)
                        {
                        const T* cpo = po;
                        cpo->_detach(/*fEscaped*/ true); // old detach
                        }
                    else
                        {
                        po->_detach(/*fEscaped*/ true); // old detach
                        }
                    }
                m_prev = m_next = this;
                }

            SmartMember::onEscape(fEscaped);
            }

        /**
        * {@inheritDoc}
        */
        virtual size64_t retained() const
            {
            ValueView v = get();
            return v == NULL
                    ? 0
                    : v->sizeOf(/*fDeep*/ true);
            }

    // ----- data members ---------------------------------------------------

    protected:
       /**
        * The referenced Object.
        */
        T* m_po;


    // ----- friends --------------------------------------------------------

    /**
    * @internal
    */
    template<class D, class H> friend D cast(MemberHandle<T>& mh, bool fThrow);

    /**
    * @internal
    */
    friend class SynchronizedMemberReadBlock;

    /**
    * @internal
    */
    friend class SynchronizedMemberWriteBlock;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given MemberHandle to the
* specified stream.
*
* @param out  the stream used to output the description
* @param mh   the MemberHandle to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const MemberHandle<T>& mh)
    {
    out << (typename T::View) mh;
    return out;
    }

/**
* Assign the specified handle to NULL.
*
* @param mh the handle to clear
*/
template<class T> void clear_handle(MemberHandle<T>& mh)
    {
    mh = NULL;
    }

/**
* Return true if the supplied handle equals NULL.
*
* @param mh  the member handle to test
*
* @return true iff the supplied handle equals NULL
*/
template<class T>
bool is_null(const MemberHandle<T>& mh)
    {
    return mh == NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the MemberHandle
* to a the specified handle/view type.
*
* @param mh      the MemberHandle from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(MemberHandle<T>& mh, bool fThrow = true)
    {
    return cast<D>((typename MemberHandle<T>::ValueHandle) mh, fThrow);
    }

/**
* Perform a dynamic cast the pointer associated with the MemberHandle
* to a the specified handle/view type.
*
* @param mh      the MemberHandle from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const MemberHandle<T>& mh, bool fThrow = true)
    {
    return cast<D>((typename MemberHandle<T>::ValueView) mh, fThrow);
    }

/**
* Perform an instanceof check on a MemberHandle.
*
* @param mh  the MemberHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(MemberHandle<T>& mh)
    {
    return NULL != cast<D>(mh, false);
    }

/**
* Perform an instanceof check on a MemberHandle.
*
* @param mh  the MemberHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const MemberHandle<T>& mh)
    {
    return NULL != cast<D>(mh, false);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_MEMBER_HANDLE_HPP
