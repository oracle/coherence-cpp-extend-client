/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TYPED_HANDLE_HPP
#define COH_TYPED_HANDLE_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/ChainedHandleElement.hpp"

#include <functional>
#include <ostream>
#include <stddef.h>
#include <typeinfo>

COH_OPEN_NAMESPACE2(coherence,lang)

class Object;

/**
* Reference counting "smart pointer" class for use with Coherence managed
* objects. Managed objects manage their own life cycle, rather the relying on
* user code to identify when they should be deleted.  The TypedHandle class
* can be used with any class implementing the coherence::lang::Object
* interface.
*
* By convention each Object implementation will include two typedefs for
* TypedHandle to make their usage easier. For instance the class "Foo" would
* define Foo::Handle as a TypedHandle<Foo>. The second typedef is called a
* View, and is analogous to a pointer to a const, that is Foo::View is
* TypedHandle<const Foo>.
*
* TypedHandles are not thread-safe, and should not be used in a multi-threaded
* context even if protected by external synchronization. They should only be
* used as local variables. The most common place where thread-safe handles are
* needed is for object data members. For these cases the Coherence API
* includes thread-safe handles/views, namely MemberHandle/View and
* WeakHandle/View.
*
* @see Object
* @see MemberHandle
* @see MemberView
* @see WeakHandle
* @see WeakView
* @see FinalHandle
* @see FinalView
*
* @author mf 2007.07.05
*/
template<class T>
class TypedHandle
    : public ChainedHandleElement
    {
    // ----- typedefs -------------------------------------------------------

    public:
        /**
        * A concrete object type that is referenced by this handle.
        */
        typedef T ValueType;


    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a handle that references NULL.
        */
        TypedHandle<T>()
            : ChainedHandleElement(constness<T>::applied), m_o(NULL)
            {
            }

        /**
        * Construct a handle that references given Object.
        *
        * @param po a pointer to the Object to reference
        */
        TypedHandle<T>(T* po)
            : ChainedHandleElement(constness<T>::applied),
              m_o(po == NULL || po->_attach(/*fEscaped*/ false) == NULL
                    ? NULL : po)
            {
            }

        /**
        * Construct a handle which is linked to another chained element type.
        * This is used for chaining Holders to Handles.
        *
        * @param po    the pointer to assign from
        * @param that  the "owner" of that pointer
        */
        TypedHandle<T>(T* po, const ChainedHandleElement& that)
            : ChainedHandleElement(that, constness<T>::applied), m_o(po)
            {
            }

        /**
        * Construct a handle from a handle to a derived type. New handle will
        * reference the same Object that the given handle.
        *
        * @param that another handle
        */
        template<class DT> TypedHandle<T>(const TypedHandle<DT>& that)
            : ChainedHandleElement(that, constness<T>::applied),
              m_o(get_pointer(that))
            {
            //      *** PLEASE READ IF YOU GET BUILD ERRORS HERE ****
            // A build error in this call indicates that you've attempted to
            // assign two non-compatible handle types, or simply trying to
            // assign a View to a Handle.  Check your compiler output which
            // should state what line in your triggered this template
            // instantiation, that is the code which has the error.
            }

        /**
        * Construct a handle from another handle. New handle will reference
        * the same Object that the given handle.
        *
        * @param that another handle
        */
        TypedHandle<T>(const TypedHandle<T>& that)
            : ChainedHandleElement(that, constness<T>::applied),
              m_o(that.get())
            {
            }

        /**
        * Destruct this handle.
        */
        ~TypedHandle<T>()
            {
            // manually inlined unlink logic allows use of constness<T>::applied
            // which should result in the elseif being compiled out for Views
            if (this == m_prev /* implies && this == m_next */)
                {
                if (m_o) performAction(action_detach);
                }
            // COH-9499 - removed m_prev and m_next m_fView pre-checks due to
            // compiler issues with GCC 4.1
            else if (m_o && !constness<T>::applied && scan())
                {
                performAction(action_flip);
                }
            // else; action_none

            // unlink self from chain
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;
            }


    // ----- operators ------------------------------------------------------

    public:
        /**
        * Dereference this handle, returning <tt>T&</tt>.
        *
        * @return a raw <tt>T&</tt> reference to the referenced Object
        *
        * @throws NullPointerException if the this handle is @c NULL
        */
        T& operator*() const
            {
            T* po = m_o;
            if (NULL == po)
                {
                coh_throw_npe(typeid(T));
                }
            return *po;
            }

        /**
        * Dereference this handle, returning <tt>T*</tt>.
        *
        * @return a raw <tt>T*</tt> pointer to the referenced Object
        *
        * @throws NullPointerException if the this handle is @c NULL
        */
        T* operator->() const
            {
            T* po = m_o;
            if (NULL == po)
                {
                coh_throw_npe(typeid(T));
                }
            return po;
            }

        /**
        * Assign this TypedHandle another Object to reference.
        */
        TypedHandle<T>& operator=(const TypedHandle<T>& h)
            {
            set(h.get(), h);
            return *this;
            }

        /**
        * Assign this TypedHandle from a handle to a derived type.
        */
        template<class DT>
        TypedHandle<T>& operator=(const TypedHandle<DT>& h)
            {
            //      *** PLEASE READ IF YOU GET BUILD ERRORS HERE ****
            // A build error in this call indicates that you've attempted to
            // assign two non-compatible handle types, or simply trying to
            // assign a View to a Handle.  Check your compiler output which
            // should state what line in your triggered this template
            // instantiation, that is the code which has the error.

            set(get_pointer(h), h);
            return *this;
            }

        /**
        * Compare two handles with "less-than" predicate.  This is only an
        * identity based comparison, and does not take the value into account.
        */
        template<class AT>
        bool operator<(const TypedHandle<AT>& h) const
            {
            return ((const Object*) m_o) < ((const Object*) get_pointer(h));
            }


    // ----- helper methods -------------------------------------------------

    protected: // protected to prevent easily misunderstandings such as
               // hMap.get() which might be interpreted as a call on a map

        /**
        * Return pointer without null checking.
        * Does NOT throw NullPointerException.
        *
        * One should use <code>h.get()</code> to get raw T* pointer.
        * Please do not use neither <code>&*h</code> (as it may throw
        * NullPointerException) nor <code>(T*)h</code> (as it may perform
        * unsafe unchecked type cast).
        */
        T* get() const
            {
            return m_o;
            }

        /**
        * Set the TypedHandle to attach to the supplied pointer.
        * Any existing attachment will be detached.
        *
        * @param o the address of the object to point to or NULL
        */
        void set(T* o, const ChainedHandleElement& that)
            {
            // manually inlined link logic allows use of constness<T>::applied
            // which should result in the elseif being compiled out for Views
            if (this == m_prev /* implies && this == m_next */)
                {
                if (m_o) performAction(action_detach);
                }
            // COH-9499 - removed m_prev and m_next m_fView pre-checks due to
            // compiler issues with GCC 4.1
            else if (m_o && !constness<T>::applied && scan())
                {
                performAction(action_flip);
                }
            // else; action_none

            // unlink self from chain
            m_prev->m_next = m_next;
            m_next->m_prev = m_prev;

            // link self into new chain
            m_prev = &that;
            m_next = that.m_next;

            m_next->m_prev = that.m_next = this;

            m_o = o;
            }

        /**
        * Perform the specified action.
        *
        * @param nAction  the action to perform
        */
        void performAction(Action nAction)
            {
            switch (nAction)
                {
                case action_flip:
                    ((const T*) m_o)->_attach(/*fEscaped*/ false);
                    // fall through
                case action_detach:
                    m_o->_detach(/*fEscaped*/ false);
                    // fall through
                case action_none:
                default:
                    break;
                }
            }

    // ----- functors -------------------------------------------------------

    protected:
        /**
        * The binary functor that compares objects pointed by the handles
        * with the "less-than" operator.
        */
        template<class RT>
        class value_less_functor
            : public std::binary_function<RT, RT, bool>
            {
            public:
                /**
                * Compare values pointed by the specified handles with
                * "less-than" operator.
                */
                bool operator()(const TypedHandle<RT>& h1,
                                const TypedHandle<RT>& h2) const
                    {
                    return h1 && h1->operator<(h2);
                    }
            };

        /**
        * The binary functor that compares values pointed by the handles for
        * equality.
        */
        template<class RT>
        class value_equal_functor
            : public std::binary_function<RT, RT, bool>
            {
            public:
                /**
                * Compare values pointed by the specified handles for
                * equality.
                */
                bool operator()(const TypedHandle<RT>& h1,
                                     const TypedHandle<RT>& h2) const
                    {
                    return h1 == h2 || (h1 && h1->equals(h2));
                    }
                };

        /**
        * Value based hash functor which is compatible with both the SGI and
        * MS style hash containers.
        */
        template<class RT>
        class value_hash_functor
            : public std::unary_function<RT, size32_t>
            {
            public:
                // parameters for MS style hash table
                enum {bucket_size = 4, min_buckets = 8};

            /**
            * Hash computation function for MS and SGI style hash containers.
            *
            * @param h the handle to hash
            *
            * @return the hash of the handle
            */
            size32_t operator()(const TypedHandle<RT>& h) const
                {
                return h ? h->hashCode() : 0;
                }

            /**
            * Ordering function for MS style hash containers.
            *
            * @param h1 the first handle to compare
            * @param h2 the second handle to compare
            *
            * @return true iff *h1 < *h2
            */
            bool operator()(const TypedHandle<RT>& h1,
                    const TypedHandle<RT>& h2) const
                {
                return h1 && h1->operator<(h2);
                }
            };


        /**
        * Identity based hash functor which is compatible with both the SGI
        * and MS style hash containers.
        */
        template<class RT> class identity_hash_functor
            : public std::unary_function<RT, size32_t>
            {
            public:
            // parameters for MS style hash table
            enum {bucket_size = 4, min_buckets = 8};

            /**
            * Hash computation function for MS and SGI style hash containers.
            *
            * @param h the handle to hash
            *
            * @return the hash of the handle
            */
            size32_t operator()(const TypedHandle<RT>& h) const
                {
                return (size32_t) get_pointer(h);
                }

            /**
            * Ordering function for MS style hash containers.
            *
            * @param h1 the first handle to compare
            * @param h2 the second handle to compare
            *
            * @return true iff h1 < h2
            */
            bool operator()(const TypedHandle<RT>& h1,
                            const TypedHandle<RT>& h2) const
                {
                return get_pointer(h1) < get_pointer(h2);
                }
            };

    public:
        /**
        * Functor allowing handles to be used within STL sorted containers,
        * such as std::map and std::set, and to have their equivalence checks
        * delegated to the Objects they reference.
        */
        typedef value_less_functor<T> value_compare;

        /**
        * Functor allowing handles to be used within hash containers, such as
        * the SGI and MS style hash_map and hash_set, and to have their
        * equivalence checks delegated to the Objects they reference.
        */
        typedef value_equal_functor<T> value_equal;

        /**
        * Functor allowing handles to be used within hash containers, such as
        * the SGI as MS style hash_map and hash_set, and to have their hash
        * computation be based on the value of the referenced Object.
        */
        typedef value_hash_functor<T> value_hasher;

        /**
        * Functor allowing handles to be used within hash containers, such as
        * the SGI as MS style hash_map and hash_set, and to have their hash
        * computation be based on their identity, i.e. the reference they
        * contain.
        */
        typedef identity_hash_functor<T> identity_hasher;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * Pointer to the referenced object.
        */
        T* m_o;


    // ---- friends ---------------------------------------------------------

    /**
    * @internal
    */
    template<class O> friend O* get_pointer(const TypedHandle<O>& h);

    /**
    * @internal
    */
    template<class D, class O> friend D cast(const TypedHandle<O>& th,
            bool fThrow);

    /**
    * @internal
    */
    template<class> friend class TypedHandle;
    };


// ----- non-member operators and functions ---------------------------------

/**
* Output a human-readable description of the given TypedHandle<T> to the
* specified stream.
*
* @param out  the stream used to output the description
* @param v    the TypedHandle<T> to describe
*
* @return the supplied stream
*/
template <typename Char, typename Traits, class T>
COH_INLINE std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& out, const TypedHandle<T>& v)
    {
    if (NULL == v)
        {
        out << "NULL";
        }
    else
        {
        out << *v;
        }
    return out;
    }

/**
* @internal
*
* Return the pointer associated with the TypedHandle.
*
* The lifetime of the Object referenced by the returned pointer
* cannot be guaranteed beyond the lifetime of the Handle from
* which it was obtained. Additionally if the associated handle is reassigned
* the lifetime of the referenced Object is also undefined.
*
* @param h the Handle to used in retrieving the pointer.
*
* @return the pointer associated with the TypedHandle.
*/
template<class T>
T* get_pointer(const TypedHandle<T>& h)
    {
    return h.get();
    }

/**
* Assign the specified handle to NULL. In most cases h = NULL is sufficient
* but for some handle types there may be ambiguity which would require
* knowledge of the Handle type, i.e. <code>hString = (String::Handle) NULL;
* </code>. While the prior example will work just fine
* <code>clear_handle(hString)</code> performs the same function without
* requiring the explicit type cast in user code, and works the same for all
* handle types.
*
* @param h the handle to clear
*/
template<class T>
void clear_handle(TypedHandle<T>& h)
    {
    h = NULL;
    }

/**
* Return true if the supplied handle equals NULL.
*
* @param h  the handle to test
*
* @return true iff the supplied handle equals NULL
*/
template<class T>
bool is_null(const TypedHandle<T>& h)
    {
    return h == NULL;
    }

/**
* Perform a dynamic cast the pointer associated with the TypedHandle
* to a the specified handle/view type.
*
* @param th      the TypedHandle from which to perform the cast
* @param fThrow  true if an exception is to be thrown on a failed cast
*
* @return the casted pointer, or NULL if the cast fails and fThrow is false
*
* @throws ClassCastException if the cast fails and fThrow of is true
*/
template<class D, class T>
D cast(const TypedHandle<T>& th, bool fThrow = true)
    {
    if (NULL == th)
        {
        return D(); // NULL
        }

    typedef typename D::ValueType DT;
    typedef typename DT::this_spec DTS;
    DT* pD;
    if (assignment<DT, T>::allowed)
        {
        // free compile time cast
        pD = assignment<DT, T>::safe(get_pointer(th));
        }
    else // runtime cast
        {
        pD = (DT*) th->_cast(COH_CLASS_ID(DTS));
        }

    if (NULL == pD)
        {
        if (fThrow)
            {
            coh_throw_class_cast(typeid(DT), typeid(*th));
            }
        return D(); // NULL
        }

    if (constness<T>::applied)
        {
        // from View to View or Holder(View)
        return TypedHandle<DT>(pD, th);
        }

    // from Handle to Handle, View, or Holder(Handle)
    TypedHandle<typename DT::Handle::ValueType> hD(
            const_cast<typename DT::Handle::ValueType*>(pD), th);
    return hD;
    }

/**
* Perform an instanceof check on a handle or view.
*
* @param th  the TypedHandle from which to perform the test
*
* @return true if the supplied handle is an instance of the specified type
*/
template<class DT, class T>
bool instanceof(const TypedHandle<T>& th)
    {
    typedef typename DT::ValueType::this_spec DTS;
    return NULL != th &&
        (assignment<typename DT::ValueType, T>::allowed ||
         th->_cast(COH_CLASS_ID(DTS)));
    }

COH_CLOSE_NAMESPACE2

#endif // COH_TYPED_HANDLE_HPP
