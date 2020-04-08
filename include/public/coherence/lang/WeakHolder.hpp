/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WEAK_HOLDER_HPP
#define COH_WEAK_HOLDER_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/Object.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"
#include "coherence/lang/MemberHolder.hpp"
#include "coherence/lang/WeakReference.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* WeakHolders are a TypedHolder like wrapper around WeakReferences.
* WeakReferences allow one Object to safely reference another without blocking
* it from being destroyed. WeakRefereces are necessary when building Object
* graphs to avoid leaking the Object graph due to cyclical references.
*
* WeakReferences are automatically NULL'd out when their referenced Object
* is destroyed. Additionally if the weakly referenced Object becomes only
* referenced via Views, then the WeakReference and corresponding WeakHandle
* will only return View to the Object, if an attempt is made to obtain a
* Handle a NULL will be returned instead.
*
* WeakHolders are wrappers for MemberHolder, and provide a thread-safe handle
* implementation.  As such they require references to both the referenced
* "child" Object, as well as the owning "parent" Object. As with MemberHolder
* the parent object must inherit from Object.
*
* Note: In the rare case that a WeakHolder is declared via the mutable
*       keyword, the WeakHolder must be informed of this fact by setting
*       fMutable to true during construction.
*
* @see MemberHolder
*
* @author mf  2008.12.09
*/
template<class T>
class WeakHolder
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

        /**
        * The associated non-weak Holder type.
        */
        typedef typename T::Holder ValueHolder;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a WeakHolder referencing NULL.
        *
        * @param oGuardian  reference to the WeakHolder's parent
        */
        WeakHolder(const Object& oGuardian)
            : m_ohWeak(oGuardian, WeakReference::valueOf(NULL)),
              m_cpWeak(NULL)
            {
            }

        /**
        * Construct a WeakHolder referencing an object.
        *
        * @param oGuardian  reference to the WeakHolder's parent
        * @param that       the object to reference
        */
        WeakHolder(const Object& oGuardian, const TypedHolder<T>& that)
            : m_ohWeak(oGuardian, WeakReference::valueOf(that)),
              m_cpWeak(get_pointer(that))
            {
            }

        /**
        * Construct a WeakHolder referencing an object.
        *
        * @param oGuardian  reference to the WeakHolder's parent
        * @param that       the object to reference
        * @param fMutable   true if the member is declared as mutable, false
        *                   if declared as const
        */
        WeakHolder(const Object& oGuardian, const TypedHolder<T>& that,
                bool fMutable)
            : m_ohWeak(oGuardian, WeakReference::valueOf(that), fMutable),
              m_cpWeak(get_pointer(that))
            {
            }


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Re-assign the WeakHolder to weakly reference a new Object.
        *
        * @param that  the object to reference
        */
        WeakHolder& operator=(const TypedHolder<T>& that)
            {
            WeakReference::Holder ohRef  = WeakReference::valueOf(that);
            const T*              cpThat = get_pointer(that);

            // update m_ohWeak and m_cpWeak under a single write lock
                {
                SynchronizedMemberWriteBlock guard(get_guardian(m_ohWeak));
                guard.setMember(m_ohWeak, ohRef);
                m_cpWeak = cpThat;
                }

            return *this;
            }

        /**
        * Re-assign the WeakHolder to weakly reference a new Object.
        *
        * @param that  the object to reference
        */
        WeakHolder& operator=(const WeakHolder& that)
            {
            return operator=((ValueHolder) that);
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

            // read m_ohWeak and m_cpWeak under a single read lock
                {
                SynchronizedMemberReadBlock guard(get_guardian(m_ohWeak));
                vRef   = guard.getMember(m_ohWeak);
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
            WeakReference::Holder ohRef;
            const T*              cpWeak;

            // read m_hWeak and m_pWeak under a single read lock
                {
                SynchronizedMemberReadBlock guard(get_guardian(m_ohWeak));
                ohRef  = guard.getMember(m_ohWeak);
                cpWeak = m_cpWeak;
                }

            WeakReference::Handle hRef = cast<WeakReference::Handle>(ohRef,
                    /*fThrow*/ false);
            Object::Holder        oh   = NULL == hRef
                ? ohRef->get()
                : hRef->get();

            if (NULL == oh)
                {
                return NULL; // Object has been collected
                }
            else if (NULL == hRef)
                {
                return TypedHandle<const T>(cpWeak, oh); // View
                }
            else
                {
                return TypedHandle<T>(const_cast<T*>(cpWeak), oh); // Handle
                }
            }

        /**
        * Dereference the WeakHolder returning a non-weak View.
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
        * The Holder to the associated WeakReference.
        */
        MemberHolder<WeakReference> m_ohWeak;

        /**
        * Raw pointer to the weakly referenced object.
        */
        const T* m_cpWeak;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given
* WeakHolder<T> to the specified stream.
*
* @param out  the stream used to output the description
* @param woh   the WeakHolder<T> to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const WeakHolder<T>& woh)
    {
    typename WeakHolder<T>::ValueView v = woh;
    out << v;
    return out;
    }

/**
* Assign the specified handle to NULL.
*
* @param woh the handle to clear
*/
template<class T> void clear_handle(WeakHolder<T>& woh)
    {
    woh = NULL;
    }

/**
* Return true if the supplied handle equals NULL.
*
* @param woh  the weak handle to test
*
* @return true iff the supplied handle equals NULL
*/
template<class T>
bool is_null(const WeakHolder<T>& woh)
    {
    return woh == NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the WeakHolder
* to a the specified handle/view type.
*
* @param woh      the WeakHolder from which to perform the cast
* @param fThrow   true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow is true
*/
template<class D, class T>
D cast(const WeakHolder<T>& woh, bool fThrow = true)
    {
    return cast<D>((TypedHolder<T>) woh, fThrow);
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param woh  the WeakHolder from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class D, class T>
bool instanceof(const WeakHolder<T>& woh)
    {
    return NULL != cast<D>(woh, false);
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WEAK_HOLDER_HPP
