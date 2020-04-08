/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WEAK_VIEW_HPP
#define COH_WEAK_VIEW_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/MemberView.hpp"
#include "coherence/lang/WeakReference.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* WeakViews are a TypedHandle like wrapper around WeakReferences.
* WeakReferences allow one Object to safely reference another without blocking
* it from being destroyed. WeakReferences are necessary when building Object
* graphs to avoid leaking the Object graph due to cyclical references.
*
* WeakReferences are automatically NULL'd out when their referenced Object
* is destroyed. Additionally if the weakly referenced Object becomes only
* referenced via Views, then the WeakReference and corresponding WeakHandle
* will only return View to the Object, if an attempt is made to obtain a
* Handle a NULL will be returned instead.
*
* WeakViews are wrappers for MemberView, and provide a thread-safe handle
* implementation.  As such they require references to both the referenced
* "child" Object, as well as the owning "parent" Object. As with MemberView
* the parent object must inherit from Object.
*
* Note: In the rare case that a WeakView is declared via the mutable
*       keyword, the WeakView must be informed of this fact by setting
*       fMutable to true during construction.
*
* @see MemberView
*
* @author mf  2007.07.05
*/
template<class T>
class WeakView
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * The type of Object which this handle weakly references.
        */
        typedef const T ValueType;

        /**
        * The associated non-weak Handle type.
        */
        typedef typename T::Handle ValueHandle;

        /**
        * The associated non-weak View type.
        */
        typedef typename T::View ValueView;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a WeakView referencing NULL.
        *
        * @param oGuardian  reference to the WeakView's parent
        */
        WeakView(const Object& oGuardian)
            : m_vWeak(oGuardian, WeakReference::valueOf(NULL)),
              m_cpWeak(NULL)
            {
            }

        /**
        * Construct a WeakView referencing an object.
        *
        * @param oGuardian  reference to the WeakView's parent
        * @param that       the object to reference
        */
        WeakView(const Object& oGuardian, const TypedHandle<const T>& that)
            : m_vWeak(oGuardian, WeakReference::valueOf(that)),
              m_cpWeak(get_pointer(that))
            {
            }

        /**
        * Construct a WeakView referencing an object.
        *
        * @param oGuardian  reference to the WeakView's parent
        * @param that       the object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        WeakView(const Object& oGuardian, const TypedHandle<const T>& that, bool fMutable)
            : m_vWeak(oGuardian, WeakReference::valueOf(that), fMutable),
              m_cpWeak(get_pointer(that))
            {
            }


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Re-assign the WeakView to weakly reference a new Object.
        *
        * @param that  the object to reference
        */
        WeakView& operator=(const TypedHandle<const T>& that)
            {
            WeakReference::View vRef   = WeakReference::valueOf(that);
            const T*            cpThat = get_pointer(that);

            // update m_vWeak and m_cpWeak under a single write lock
                {
                SynchronizedMemberWriteBlock guard(get_guardian(m_vWeak));
                guard.setMember(m_vWeak, vRef);
                m_cpWeak = cpThat;
                }

            return *this;
            }

        /**
        * Re-assign the WeakView to weakly reference a new Object.
        *
        * @param that  the object to reference
        */
        WeakView& operator=(const WeakView& that)
            {
            return operator=((ValueView) that); // assign from snapshot
            }

        /**
        * Return a non-weak View to the weakly referenced Object, or a
        * View to NULL if the weakly referenced Object has been
        * destroyed.
        *
        * @return ValueView
        */
        operator ValueView() const
            {
            WeakReference::View vRef;
            const T*            cpWeak;

            // read m_vWeak and m_cpWeak under a single read lock
                {
                SynchronizedMemberReadBlock guard(get_guardian(m_vWeak));
                vRef   = guard.getMember(m_vWeak);
                cpWeak = m_cpWeak;
                }

            Object::View v = vRef->get();
            return NULL == v
                ? TypedHandle<const T>() // Object has been collected
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
        * Return a TypedHolder to the referenced Object.
        *
        * @return a TypedHolder to the referenced Object
        */
        template<class PT>
        operator TypedHolder<PT>() const
            {
            return (ValueView) *this;
            }

        /**
        * Dereference the WeakView returning a non-weak View..
        *
        * If the non-weak View is non-NULL then it is guaranteed
        * that the Object will continue to exist for at least the
        * life of the returned View.
        *
        * @return a non-weak View to the referenced Object
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

    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The View to the associated WeakReference.
        */
        MemberView<WeakReference> m_vWeak;

        /**
        * Raw pointer to the weakly referenced object.
        */
        const T* m_cpWeak;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given
* WeakView<T> to the specified stream.
*
* @param out  the stream used to output the description
* @param wv   the WeakView<T> to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const WeakView<T>& wv)
    {
    typename WeakView<T>::ValueView v = wv;
    out << v;
    return out;
    }

/**
* Assign the specified handle to NULL.
*
* @param wv the handle to clear
*/
template<class T> void clear_handle(WeakView<T>& wv)
    {
    wv = NULL;
    }

/**
* Return true if the supplied handle equals NULL.
*
* @param wv  the weak handle to test
*
* @return true iff the supplied handle equals NULL
*/
template<class T>
bool is_null(const WeakView<T>& wv)
    {
    return wv == NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the WeakView
* to a the specified handle/view type.
*
* @param wv      the WeakView from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const WeakView<T>& wv, bool fThrow = true)
    {
    return cast<D>((typename WeakView<T>::ValueView) wv, fThrow);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param wv  the WeakView from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const WeakView<T>& wv)
    {
    return NULL != cast<D>(wv, false);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WEAK_VIEW_HPP
