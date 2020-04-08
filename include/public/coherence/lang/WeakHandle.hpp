/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WEAK_HANDLE_HPP
#define COH_WEAK_HANDLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/MemberHandle.hpp"
#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"
#include "coherence/lang/WeakReference.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* WeakHandles are a TypedHandle like wrapper around WeakReferences.
* WeakReferences allow one Object to safely reference another without blocking
* it from being destroyed. WeakReferences are necessary when building Object
* graphs to avoid leaking the Object graph due to cyclic references.
*
* WeakReferences are automatically NULL'd out when their referenced Object
* is destroyed. Additionally if the weakly referenced Object becomes only
* referenced via Views, then the WeakReference and corresponding WeakHandle
* will only return View to the Object, if an attempt is made to obtain a
* Handle a NULL will be returned instead.
*
* WeakHandles behave like MemberHandles by transferring their constness to
* their referenced Object. As with MemberHandle WeakHandles are a thread-safe
* handle implementation.  As such they require references to both the
* referenced "child" Object, as well as the owning "parent" Object. As with
* MemberView the parent object must inherit from Object.
*
* If the weakly referenced Object will always be referenced as a View then
* using the WeakView class would be more appropriate
*
* Note: In the rare case that a WeakHandle is declared via the mutable
*       keyword, the WeakHandle must be informed of this fact by setting
*       fMutable to true during construction.
*
* @see MemberHandle
* @see WeakView
*
* @author mf  2007.07.05
*/
template<class T>
class WeakHandle
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The type of Object which this handle weakly references.
        */
        typedef T ValueType;

        /**
        * The associated non-weak Handle type.
        */
        typedef typename T::Handle ValueHandle;

        /**
        * The associated non-weak View type.
        */
        typedef typename T::View ValueView;

        /**
        * The MemberHandle used to hold the internal WeakReference.
        */
        typedef MemberHandle<WeakReference> InnerHandle;

        /**
        * Result type for a non-const get operation.
        */
        typedef ValueHandle GetType;

        /**
        * Result type for a non-const get operation.
        */
        typedef ValueView ConstGetType;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Default constructor.
        */
        WeakHandle(const Object& oGuardian)
            : m_hWeak(oGuardian), m_pWeak(NULL)
            {
            }

        /**
        * Construct a WeakHandle from a normal Handle.
        *
        * @param oGuardian  the self() on the object this data member is a
        *                   member of
        * @param that       the object to reference
        */
        WeakHandle(const Object& oGuardian, const TypedHandle<T>& that)
            : m_hWeak(oGuardian, NULL == that ? NULL : that->_attachWeak()),
              m_pWeak(get_pointer(that))
            {
            }

        /**
        * Construct a WeakHandle from a normal Handle.
        *
        * @param oGuardian  the self() on the object this data member is a
        *                   member of
        * @param that       the object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        WeakHandle(const Object& oGuardian, const TypedHandle<T>& that, bool fMutable)
            : m_hWeak(oGuardian, NULL == that ? NULL : that->_attachWeak(),
                    fMutable),
              m_pWeak(get_pointer(that))
            {
            }

    private:
        /**
        * Blocked copy constructor.
        */
        WeakHandle(const WeakHandle& wh);


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Re-assign the WeakHandle to weakly reference a new Object.
        *
        * @param that  the object to reference
        */
        WeakHandle& operator=(const TypedHandle<T>& that)
            {
            T*                    pThat = get_pointer(that);
            WeakReference::Handle hRef  = NULL == that
                ? WeakReference::Handle(NULL)
                : that->_attachWeak();

            // update m_hWeak and m_pWeak under a single write lock
                {
                SynchronizedMemberWriteBlock guard(get_guardian(m_hWeak));
                guard.setMember(m_hWeak, hRef);
                m_pWeak = pThat;
                }

            return *this;
            }

        /**
        * Assign this WeakHandle to reference the same Object as the specified
        * WeakHandle.
        *
        * @param that  the object to reference
        *
        * @return a reference to this MemberHandle
        */
        WeakHandle& operator=(WeakHandle& that)
            {
            return operator=((ValueHandle) that); // assign from snapshot
            }

        /**
        * Return a non-weak Handle to the weakly referenced Object, or a
        * Handle to NULL if the weakly referenced Object has been destroyed
        * or is now only referenced via Views.
        *
        * @return ValueHandle
        */
        operator ValueHandle()
            {
            WeakReference::Handle hRef;
            T*                    pWeak;

            // read m_hWeak and m_pWeak under a single read lock
                {
                SynchronizedMemberReadBlock guard(get_guardian(m_hWeak));
                hRef  = guard.getMember(m_hWeak);
                pWeak = m_pWeak;
                }

            if (NULL == hRef)
                {
                return NULL;
                }

            // utilize pWeak to avoid cost of dynamic cast
            Object::Handle h = cast<Object::Handle>(hRef->get(),
                    /*fThrow*/ false);
            return NULL == h
                ? TypedHandle<T>()
                : TypedHandle<T>(pWeak, h);
            }

        /**
        * Return a non-weak View to the weakly referenced Object, or a
        * Handle to NULL if the weakly referenced Object has been
        * destroyed.
        *
        * @return ValueHandle
        */
        operator ValueView() const
            {
            WeakReference::View vRef;
            const T*            cpWeak;

            // read m_hWeak and m_pWeak under a single read lock
                {
                SynchronizedMemberReadBlock guard(get_guardian(m_hWeak));
                vRef   = guard.getMember(m_hWeak);
                cpWeak = m_pWeak;
                }

            if (NULL == vRef)
                {
                return NULL;
                }

            // utilize cpWeak to avoid cost of dynamic cast
            Object::View v = vRef->get();
            return NULL == v
                ? TypedHandle<const T>()
                : TypedHandle<const T>(cpWeak, v);
            }

        /**
        * Return a View to the referenced Object.
        *
        * @return a View to the referenced Object
        */
        template<class PT>
        operator TypedHandle<const PT>() const
            {
            return (ValueView) *this;
            }

        /**
        * Return a Handle to the referenced Object.
        *
        * @return a Handle to the referenced Object
        */
        template<class PT>
        operator TypedHandle<PT>()
            {
            return (ValueHandle) *this;
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
            return (ValueView) *this;
            }

        /**
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        */
        template<class PT>
        operator TypedHolder<PT>()
            {
            return (ValueHandle) *this;
            }

        /**
        * Dereference the WeakHandle returning a non-weak Handle.
        *
        * If the non-weak Handle is non-NULL then it is guaranteed
        * that the Object will continue to exist for at least the
        * life of the returned handle.
        *
        * @return ValueHandle
        */
        ValueHandle operator->()
            {
            return (ValueHandle) *this;
            }

        /**
        * Dereference the WeakHandle returning a non-weak View.
        *
        * If the non-weak Handle is non-NULL then it is guaranteed
        * that the Object will continue to exist for at least the
        * life of the returned View.
        *
        * @return ValueHandle
        */
        ValueView operator->() const
            {
            return (ValueView) *this;
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
            return *operator->();
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
            return *operator->();
            }

    private:
        /**
        * Blocked assignment operator.
        */
        WeakHandle<T>& operator=(const WeakHandle<T>&);


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The Handle to the associated WeakReference.
        */
        InnerHandle m_hWeak;

        /**
        * Raw pointer to the weakly referenced object.
        */
        T* m_pWeak;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given WeakHandle<T> to the
* specified stream.
*
* @param out  the stream used to output the description
* @param wh   the WeakHandle<T> to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const WeakHandle<T>& wh)
    {
    typename WeakHandle<T>::ValueView v = wh;
    out << v;
    return out;
    }

/**
* Assign the specified handle to NULL.
*
* @param wh the handle to clear
*/
template<class T> void clear_handle(WeakHandle<T>& wh)
    {
    wh = NULL;
    }

/**
* Return true if the supplied handle equals NULL.
*
* @param wh  the weak handle to test
*
* @return true iff the supplied handle equals NULL
*/
template<class T>
bool is_null(const WeakHandle<T>& wh)
    {
    return wh == NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the WeakHandle
* to a the specified handle/view type.
*
* @param wh     the WeakHandle from which to perform the cast
* @param fTest  true for instanceof style test
*
* @return the casted pointer, or NULL if the cast fails and fTest is true
*
* @throws ClassCastException if the cast fails and fTest of is false
*/
template<class D, class T>
D cast(WeakHandle<T>& wh, bool fTest = false)
    {
    return cast<D>((typename WeakHandle<T>::ValueHandle) wh, fTest);
    }

/**
* Perform a dynamic cast the pointer associated with the WeakHandle
* to a the specified handle/view type.
*
* @param wh      the WeakHandle from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const WeakHandle<T>& wh, bool fThrow = true)
    {
    return cast<D>((typename WeakHandle<T>::ValueHandle) wh, fThrow);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param wh  the WeakHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const WeakHandle<T>& wh)
    {
    return NULL != cast<D>(wh, false);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param wh  the WeakHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(WeakHandle<T>& wh)
    {
    return NULL != cast<D>(wh, false);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WEAK_HANDLE_HPP
