/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_OBJECT_HPP
#define COH_OBJECT_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/cloneable_spec.hpp"
#include "coherence/lang/LifeCycle.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"
#include "coherence/native/NativeAtomic32.hpp"

#include <ostream>

COH_OPEN_NAMESPACE2(coherence,native)
class NativeCondition;
COH_CLOSE_NAMESPACE2

COH_OPEN_NAMESPACE2(coherence,lang)

class Object;
class SmartMember;
class String;
class WeakReference;

using coherence::native::NativeAtomic32;
using coherence::native::NativeCondition;


/**
* Object is the base class for all Coherence managed objects.
*
* The interface for Object provides support basic operations such as equality,
* hashing, printing, cloning, synchronization, and notification.
*
* Objects have automatically managed memory, through built-in thread-safe
* reference counting. The reference count is maintained through the use of
* smart-pointers, known as handles. All references to an Object should be
* maintained through a handle rather then a raw pointer, or C++ reference.
*
* Each managed class includes inner definitions of handles which can be used
* to reference instance of that class, as well as derived class instances.
* These definitions provide functionality corresponding to const and non-const
* pointers. The names for these handles are Handle for the non-const, View for
* the const pointer equivalent, and Holder for a hybrid. The Holder type acts
* like a View but allows for a safe down-cast to a Handle, which will only
* succeed if the Holder was assigned from a Handle, and will fail with a
* ClassCastException if it had been assigned from a View. Note that in
* documentation the term "handle" is used to describe any handle type
* including Handle/View/Holder, while "Handle" refers to the specific
* "Handle" type.
*
* The handles are aware of the inheritance hierarchy, allowing direct
* assignment from a handle from a derived class to a handle of a base class.
* These up-casts are automatic, and an explicit cast operation is rarely
* needed, except to resolve compiler type ambiguities. Assignment from a View
* to a Handle is also automatic as the const-interface of a class can
* naturally be considered a base-class. Assignment of a handle to a derived
* type is not automatic, and requires an explicit down-cast via the coherence
* cast<H>(h) function. The template parameter is the desired handle type, while
* the function parameter is the handle to cast. If the supplied handle is
* non-NULL and is not an instance of the class associated with H, a
* ClassCastException will be thrown. There is a corresponding instanceof<H>(h)
* method which can be used to perform the type-check without the risk of
* triggering an exception. The use of instanceof, is optional, and is only
* needed if the type is actually in question.
*
* @code
* Object::Handle hObject = ...
* Foo::Handle    hFoo;
*
* if (instanceof<Foo::Handle>(hObject))
*   {
*   hFoo = cast<Foo::Handle>(hObject);
*   }
* @endcode
*
* These convenience handles are not thread-safe, and should not be used in a
* multi-threaded context. They should only be used as local variables, though
* they are free to reference objects shared by many threads. The most common
* place where thread-safe handles are needed is for data members of managed
* objects. For these cases the Coherence API includes a variety of thread-safe
* handles/views. It is necessary that any class which extends from Object use
* thread-safe handles when referencing other managed Objects.
*
* By convention managed objects will also block direct external access to
* their constructor and destructor. Constructors are accessed via public
* static "create" factory methods. The copy constructor is accessed via the
* public virtual "clone" method. Destructors are protected and destruction
* is triggered automatically when the internal reference count reaches zero.
*
* @see class_spec     for defining non-cloneable classes
* @see cloneable_spec for defining cloneable classes
* @see abstract_spec  for defining abstract classes
* @see interface_spec for defining interfaces
* @see throwable_spec for defining exceptions
* @see MemberHandle   for thread-safe data member handles
* @see MemberView     for thread-safe data member views
* @see MemberHolder   for thread-safe data member holders
* @see FinalHandle    for thread-safe "const" data member handles
* @see FinalView      for thread-safe "const" data member views
* @see FinalHolder    for thread-safe "const" data member holders
* @see WeakHandle     for thread-safe weak data member handles
* @see WeakView       for thread-safe weak data member views
* @see WeakHolder     for thread-safe weak data member holders
*
* @author mf 2007.07.05
*/
class COH_EXPORT Object
    : public cloneable_spec<Object,
        extends<> >
    {
    friend class factory<Object>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Construct a new Object.
        */
        Object();

        /**
        * Copy constructor.
        */
        Object(const Object& that);

    private:
        /**
        * Blocked assignment operator.
        *
        * @internal
        */
        Object& operator=(const Object&);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * Return true iff the specified Object is "equal" to this Object.
        *
        * This method implements an <i>equivalence relation</i> on Objects:
        * <ul>
        * <li>It is <i>reflexive</i>: for any non-null handle
        *     <code>h</code>, <code>h->equals(h)</code> must return
        *     <code>true</code>.
        * <li>It is <i>symmetric</i>: for any non-null handles
        *     <code>h1</code> and <code>h2</code>, <code>h1->equals(h2)</code>
        *     should return <code>true</code> if and only if
        *     <code>h2->equals(h1)</code> returns <code>true</code>.
        * <li>It is <i>transitive</i>: for any non-null handles
        *     <code>h1</code>, <code>h2</code>, and <code>h3</code>, if
        *     <code>h1->equals(h2)</code> returns <code>true</code> and
        *     <code>h2->equals(h3)</code> returns <code>true</code>, then
        *     <code>h1->equals(h3)</code> should return <code>true</code>.
        * <li>It is <i>consistent</i>: for any non-null handles
        *     <code>h1</code> and <code>h2</code>, multiple invocations of
        *     <tt>h1->equals(h2)</tt> consistently return <code>true</code>
        *     or consistently return <tt>false</tt>, provided no
        *     information used in comparisons on the objects is modified.
        * <li>If the supplied handle is <tt>NULL</tt> then <tt>false</tt>
        *     must be returned.
        * </ul>
        *
        * The default implementation is a reference equality comparison.
        *
        * @param v  the Object::View to compare against, may be <tt>NULL</tt>
        *
        * @return <tt>true</tt> iff the given handle references an Object
        *         that is "equal" to this Object
        *
        * @see #equals(Object::View v1, Object::View v2)
        */
        virtual bool equals(Object::View v) const;

        /**
        * Return a hash code value for the Object. This method is supported
        * for the benefit of hash-based containers.
        *
        * The general contract of <code>hashCode</code> is:
        * <ul>
        * <li>Whenever it is invoked on the same Object more than once
        *     during an execution of an application, the <tt>hashCode</tt>
        *     method must consistently return the same value, provided no
        *     information used in <tt>equals</tt> comparisons on the object
        *     is modified. This value need not remain consistent from one
        *     execution of an application to another execution of the same
        *     application.
        * <li>If two Objects are equal according to the <tt>equals</tt>
        *     method, then calling the <code>hashCode</code> method on each
        *     of the two Objects must produce the same value.
        * <li>It is <em>not</em> required that if two Objects are unequal
        *     according to the <tt>equals</tt> method, then calling the
        *     <tt>hashCode</tt> method on each of the two objects must
        *     produce distinct results. However, the programmer should be
        *     aware that producing distinct results for unequal objects may
        *     improve the performance of hash-based containers.
        * </ul>
        *
        * The default implementation is identity based.
        *
        * @return a hash code value for this Object
        */
        virtual size32_t hashCode() const;

        /**
        * Return a handle to a deep copy of the original Object.
        *
        * The returned clone should be sufficient decoupled from the original
        * such that further modifications to the original Object will not be
        * visible within the cloned object.  More specifically, the following
        * is expected to hold true.
        *
        * @code
        * h->clone() != h &&
        * h->clone()->equals(h) &&
        * typeid(*h) == typeid(*(h->clone()))
        * @endcode
        *
        * Note that this suggests that data members of a cloned object are
        * expected to cloned if they are tested for equality (via a call to
        * equals) within the cloned object's equals method.  If a data member
        * is compared via reference equality, or not even considered within
        * equals, then it does not necessarily need to be deeply cloned.
        *
        * Object is cloneable, but it's derived classes are not automatically
        * cloneable, and CloneNotSupportedExceptions are thrown. To be made
        * cloneable the derived class should use the cloneable_spec<> helper
        * template in its declaration, and define a protected copy constructor.
        * The derived class does not need to override this method, as that is
        * done by the cloneable_spec<> template.
        *
        * @return a copy of the original Object
        *
        * @throws CloneNotSupportedException if the object cannot be deep
        *         cloned
        *
        * @see isImmutable()
        */
        virtual Object::Handle clone() const;

        /**
        * Return <tt>true</tt> iff no further changes can be made to the
        * Object, that would effect the outcome of a call to its equals method.
        * Except for Objects which are naturally immutable (such as String),
        * being immutable generally implies that the Object is only referenced
        * via const pointers or views. Objects which hold references to child
        * Objects, may need to take the immutability of their children into
        * account when determining their own immutability.
        *
        * This extended check is not performed by the default implementation,
        * but can be integrated into the immutability checks by overriding
        * this method, as well as making use of MemberHandles to reference
        * child Objects.
        *
        * A typical derived implementation may look as follows:
        *
        * <pre>
        * bool isImmutable() const
        *     {
        *     if (m_fImmutable) // check recorded state
        *         {
        *         return true; // already marked as immutable, avoid calculation
        *         }
        *     else if (Object::isImmutable()) // ensure shallow immutability
        *         {
        *         // ensure deep immutability
        *         if (m_child1->isImmutable() && m_child2->isImmutable() ...
        *                                     && m_childN->isImmutable())
        *             {
        *             // record and return immutability
        *             return m_fImmutable = true;
        *             }
        *         // some Objects which comprise this Object are still mutable
        *         }
        *     return false;
        *     }
        * </pre>
        *
        * The default implementation return true iff the Object is only
        * referenced via const pointers and or views.
        *
        * @return true iff the Object is immutable
        */
        virtual bool isImmutable() const;

        /**
        * Deprecated as of 12.2.1
        *
        * Output a human-readable description of this Object to the given stream.
        *
        * As of 12.2.1 this method is deprecated in favor of Object::toString() which includes
        * support for unicode output and is agnostic to differences between ostream library
        * versions.
        *
        * The default implementation of toStream will simply delegate to toString.
        *
        * @deprecated 12.2.1
        */
        virtual void toStream(std::ostream& /*out*/) const
            {
            // because of the possibility of standard library changes it is unsafe for this
            // method to actually use the stream.  All built-in implementations have been
            // moved to toString, see operator<< for Object for bridging code which provides
            // minimal support for non-built-in classes which may still implement this method.

            // Note this method cannot be made safe via in-lining as it must also be virtual
            // which will almost always keep the method from being inlined.
            }

        /**
        * Output a human-readable description of this Object to the given
        * stream.
        *
        * Note that when overriding this method the return type must be TypedHandle<const String>
        * rather then String::View.  These two types are assignment compatible but not equivalent
        * and declaring the override with String::View will not be a compatible override.
        *
        * coherence::lang::operator<<(std::ostream, Object::View) is defined
        * and will call into the toString method, to output Objects.  If a
        * managed String object is desired, the COH_TO_STRING macro can be used
        * to build up a String from streamable contents and is generally how
        * toString() will be implemented.
        *
        * @code
        * Object::View vKey   = ...
        * Object::View vValue = ...
        * std::cout << vKey << " = " << vValue << std::endl;
        *
        * String::View vs = COH_TO_STRING(vKey << " = " << vValue);
        * @endcode
        *
        * The COH_TO_STRING macro is also the most common way to implement the toString method.
        * For example:
        *
        * @code
        * virtual TypedHandle<const String> Person::toString() const
        *     {
        *     return COH_TO_STRING("Name: " << f_sName << " SSN: " << f_nSSN);
        *     }
        * @endcode
        *
        * @return a string representation of this object
        */
        virtual TypedHandle<const String> toString() const;

        /**
        * Return an estimate as to the byte size of the object.
        *
        * The shallow size should not include the cost of referenced managed
        * objects, or non-fixed dynamically allocated memory. That for a
        * given object it is assumed that a call to this method will always
        * return the same value when the shallow size is requested.
        *
        * The deep size should be the shallow size plus the size of any
        * referenced objects. Unlike the shallow size the deep size may change
        * over time. The default implementation provided by Object will
        * traverse the object graph of all managed objects which it references
        * and include their sizes in the computed deep size. In general it
        * is sufficient to simply delegate to super::sizeOf(true) for deep
        * calculations.
        *
        * For classes implemented via class_spec, a variant of this method
        * will be automatically produced which will utilize sizeof(*this)
        * to compute the shallow size, and super::sizeOf(true) to compute
        * the deep size.  As such it is generally not necessary to provide
        * custom implmenetations of this method.
        *
        * @param fDeep  true if the size should include the size of referenced
        *               objects
        *
        * @return the approximate shallow byte size of the object
        */
        virtual size64_t sizeOf(bool fDeep = false) const;


    // ----- synchronization and notification interface ---------------------

    public:
        /**
        * Block the calling thread while waiting for notification.
        *
        * The caller must be synchronized on the Object's monitor when calling
        * this method. The monitor will be automatically released while the
        * thread awaits notification, and re-acquired before the call returns.
        * This method is subject to spurious wakeups, and the caller should
        * externally verify that the condition being waited for has been
        * reached.
        *
        * To synchronize on an object, the COH_SYNCHRONIZED macro is used.
        * The macro defines a block of code which is a critical section,
        * during which no other thread may acquire synchronization on the same
        * object.
        *
        * @code
        * Object::Handle hObject = ...
        * COH_SYNCHRONIZED (hObject)
        *   {
        *   while (testSomeCondition() == false)
        *     {
        *     hObject->wait();
        *     }
        *   }
        * @endcode
        *
        * @see notify
        * @see notifyAll
        */
        void wait() const;

        /**
        * Block the calling thread while waiting for notification.
        *
        * The caller must be synchronized on the Object's monitor when calling
        * this method. The monitor will be automatically released while the
        * thread awaits notification, and re-acquired before the call returns.
        * This method is subject to spurious wakeups, and the caller should
        * externally verify that the condition being waited for has been
        * reached.
        *
        * @param cMillis  the duration to wait to wait, a value of zero will
        *                 wait indefinitely for notification
        *
        * @see wait
        * @see notify
        * @see notifyAll
        */
        void wait(int64_t cMillis) const;

        /**
        * Notify a waiting thread.
        *
        * The caller must be synchronized on the Object's monitor when calling
        * this method.
        *
        * @code
        * Object::Handle hObject = ...
        * COH_SYNCHRONIZED (hObject)
        *   {
        *   setSomeCondition(true);
        *   hObject->notify();
        *   }
        * @endcode
        *
        * @see wait
        * @see notifyAll
        */
        void notify() const;

        /**
        * Notify all waiting threads.
        *
        * The caller must be synchronized on the Object's monitor when calling
        * this method.
        *
        * @see wait
        * @see notify
        */
        void notifyAll() const;

    private:
        /**
        * Enter the Object's monitor, waiting as long as necessary.
        * <p/>
        * The monitor supports recursive entry, allowing the same thread to
        * reenter the same monitor. The thread must exit the monitor the
        * same number of times, by calling the exitMonitor, for the Monitor
        * to be released.
        *
        * It is recommended that the COH_SYNCHRONIZED macro be used in place
        * of direct calls to enterMonitor/exitMonitor. The use of this macro
        * only requires public level view access to the Object
        * <pre>
        * // outside of synchronized block
        * COH_SYNCHRONIZED (vObject) // monitor entered
        *     {
        *     // critical section goes here
        *     // ...
        *     // ...
        *     } // monitor automatically exited
        * // outside of synchronized block
        * </pre>
        *
        * @see _exitMonitor
        */
        void _enterMonitor() const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());
            if (state.value.fEscaped ||
                state.value.nMonitor != LifeCycle::flat)
                {
                _enterMonitorInternal();
                }
            else
                {
                // pointer acts as a nested synchronization counter
                m_pCondition = reinterpret_cast<NativeCondition*>(
                        reinterpret_cast<size_t>(m_pCondition) + 1);
                }
            }

        /**
        * @internal
        */
        void _enterMonitorInternal() const;

        /**
        * Exit the Object's monitor.
        *
        * This must be called by the thread which locked the monitor.
        *
        * @see _enterMonitor
        */
        void _exitMonitor() const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());
            if (state.value.fEscaped ||
                state.value.nMonitor != LifeCycle::flat)
                {
                _exitMonitorInternal();
                }
            else if (m_pCondition == NULL)
                {
                coh_throw_illegal_state("Invalid monitor state");
                }
            else
                {
                // pointer acts as a nested synchronization counter
                m_pCondition = reinterpret_cast<NativeCondition*>(
                        reinterpret_cast<size_t>(m_pCondition) - 1);
                }
            }

        /**
        * @internal
        */
        void _exitMonitorInternal() const;

        /**
        * Acquire the shared read lock for the Object's data members.
        *
        * The data member read/write lock is intended for protecting small
        * sections of non-blocking code, such as primitive data-member access
        * and assignment. The read lock does support limited recursion,
        * and each call to acquire must be matched by a corresponding
        * call to release. For more advanced read/write lock support the
        * ThreadGate class should be utilized.
        *
        * The member read lock may be acquired using the
        * COH_SYNCHRONIZED_MEMBER_READ macro.
        *
        * @see SynchronizedMemberReadBlock
        * @see coherence::util::ThreadGate
        */
        void _acquireMemberReadLock() const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());
            if (state.value.fEscaped)
                {
                _acquireMemberReadLockInternal();
                }
            else if (state.value.nMemberWriteLockState ==
                     LifeCycle::gate_open &&
                     state.value.cMemberReadLock < LifeCycle::max_read_locks)
                {
                ++state.value.cMemberReadLock;
                m_atomicLifeCycleState.poke(state);
                }
            else
                {
                coh_throw_illegal_state("Invalid lock state");
                }
            }

        /**
        * Release the read lock for the Object's data members
        */
        void _releaseMemberReadLock() const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());
            if (state.value.fEscaped)
                {
                _releaseMemberReadLockInternal();
                }
            else if (state.value.nMemberWriteLockState ==
                     LifeCycle::gate_open &&
                     state.value.cMemberReadLock > 0)
                {
                --state.value.cMemberReadLock;
                m_atomicLifeCycleState.poke(state);
                }
            else
                {
                coh_throw_illegal_state("Invalid lock state");
                }
            }

        /**
         * Cross the read barrier associated with the object's read write lock.
         */
        void _readBarrier() const
            {
            m_atomicLifeCycleState.get();
            }

        /**
        * Acquire the write lock for the Object's data members.
        *
        * Unlike the member read lock, write locks do not support
        * recursion, nor do they support lock promotion from a read to a
        * write lock. Attempts to use them such a manor will result in
        * dead-lock.
        *
        * The member write lock may be acquired using the
        * COH_SYNCHRONIZED_MEMBER_WRITE macro.
        *
        * @see SynchronizedMemberWriteBlock
        */
        void _acquireMemberWriteLock() const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());
            if (state.value.fEscaped)
                {
                _acquireMemberWriteLockInternal();
                }
            else if (state.value.nMemberWriteLockState ==
                     LifeCycle::gate_open &&
                     state.value.cMemberReadLock == 0)
                {
                state.value.nMemberWriteLockState = LifeCycle::gate_closed;
                m_atomicLifeCycleState.poke(state);
                }
            else
                {
                coh_throw_illegal_state("Invalid lock state");
                }
            }

        /**
        * Release the write lock for the Object's data members.
        */
        void _releaseMemberWriteLock() const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());
            if (state.value.fEscaped)
                {
                _releaseMemberWriteLockInternal();
                }
            else if (state.value.nMemberWriteLockState ==
                     LifeCycle::gate_closed)
                {
                state.value.nMemberWriteLockState = LifeCycle::gate_open;
                m_atomicLifeCycleState.poke(state);
                }
            else
                {
                coh_throw_illegal_state("Invalid lock state");
                }
            }

        /**
         * Cross the write barrier associated with the object's read write lock.
         */
        void _writeBarrier() const
            {
            m_atomicLifeCycleState.update(0, 0);
            }

    // ----- reference counting interface -----------------------------------

    public:
        /**
        * @internal
        *
        * Increment the non-const reference count for this Object.
        *
        * NOTE: This method is intended for use by smart pointers such as
        * TypedHandle, and its usage should be limited.
        *
        * This operation must return <tt>NULL</tt> if the Object is
        * immutable.
        *
        * @param fEscaped  true if the attaching reference is visible to
        *                  multiple threads
        *
        * @return a pointer to the Object or <tt>NULL</tt> if new non-const
        *         attachments are disallowed
        *
        * @see _detach
        */
        Object* _attach(bool fEscaped = true)
            {
            if (LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped)
                {
                return _attachEscaped(fEscaped);
                }

            LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());
            unsigned int cHandle = refs.value.cHandle;
            if (fEscaped || cHandle == 0 ||
                cHandle >= LifeCycle::max_handles - LifeCycle::escaped_increment || (cHandle == 1 &&
                refs.value.nWeakReference == LifeCycle::inflated))
                {
                return _attachCaptured(fEscaped);
                }

            // inlined non-escaped attach
            refs.value.cHandle = cHandle + 1;
            m_atomicLifeCycleRefs.poke(refs);
            return this;
            }

        /**
        * @internal
        *
        * Increment the const reference count for this Object.
        *
        * NOTE: This method is intended for use by smart pointers such as
        * TypedHandle, and its usage should be limited.
        *
        * @param fEscaped  true if the attaching reference is visible to
        *                  multiple threads
        *
        * @return a constant pointer to the Object, or <tt>NULL</tt> if new
        *         const attachments are disallowed
        *
        * @see _detach
        */
        const Object* _attach(bool fEscaped = true) const
            {
            if (LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped)
                {
                return _attachEscaped(fEscaped);
                }

            LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());
            unsigned int cView  = refs.value.cView;
            unsigned int cTotal = refs.value.cHandle + cView;
            if (fEscaped || cView >= LifeCycle::max_views - LifeCycle::escaped_increment || cTotal == 0 ||
               (cTotal == 1 && refs.value.nWeakReference == LifeCycle::inflated))
                {
                return _attachCaptured(fEscaped);
                }

            // inlined non-escaped attach
            refs.value.cView = cView + 1;
            m_atomicLifeCycleRefs.poke(refs);
            return this;
            }

        /**
        * @internal
        *
        * Return a WeakReference to this Object.
        *
        * Unlike non-weak attachments, there is no corresponding _detachWeak,
        * as the return type is a handle.
        *
        * See the WeakHandle template class which will automatically manage
        * the weak attachment to the Object.
        *
        * NOTE: This method is intended for use by smart pointers such as
        * WeakHandle, and its usage should be limited.
        *
        * @see WeakReference
        * @see WeakHandle
        *
        * @return a WeakReference to this Object
        */
        TypedHandle<WeakReference> _attachWeak();

        /**
        * @internal
        *
        * Return a WeakReference View to this Object.
        *
        * Unlike non-weak attachments, there is no corresponding _detachWeak,
        * as the return type is a handle.
        *
        * See the WeakView template class which will automatically manage the
        * weak attachment to the Object.
        *
        * NOTE: This method is intended for use by smart pointers such as
        * WeakHandle, and its usage should be limited.
        *
        * @see WeakReference
        * @see WeakView
        *
        * @return a WeakReference to this Object
        */
        TypedHandle<const WeakReference> _attachWeak() const;

        /**
        * @internal
        *
        * Decrement the non-const reference count for this Object and
        * automatically delete this Object if the total reference count drops
        * to zero.
        *
        * NOTE: This method is intended for use by smart pointers such as
        * TypedHandle, and its usage should be limited.
        *
        * @see _attach
        *
        * @param fEscaped  true if the detaching reference had attached as
        *                  escaped
        */
        void _detach(bool fEscaped = true)
            {
            if (fEscaped || LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped)
                {
                _detachEscaped(fEscaped);
                return;
                }

            LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());
            unsigned int cHandle = refs.value.cHandle;
            if (cHandle <= 1 || (cHandle == 2 &&
                refs.value.nWeakReference == LifeCycle::inflated) ||
                cHandle == LifeCycle::max_handles)
                {
                _detachCaptured();
                }
            else
                {
                // inlined non-escaped detach
                refs.value.cHandle = cHandle - 1;
                m_atomicLifeCycleRefs.poke(refs);
                }
            }

        /**
        * @internal
        *
        * Decrement the const reference count for this Object and
        * automatically delete this Object if the total reference count drops
        * to zero.
        *
        * NOTE: This method is intended for use by smart pointers such as
        * TypedHandle, and its usage should be limited.
        *
        * @see _attach
        *
        * @param fEscaped  true if the detaching reference had attached as
        *                  escaped
        */
        void _detach(bool fEscaped) const
            {
            if (fEscaped || LifeCycle::State(m_atomicLifeCycleState.peek()).value.fEscaped)
                {
                _detachEscaped(fEscaped);
                return;
                }

            LifeCycle::Refs refs(m_atomicLifeCycleRefs.peek());
            unsigned int cView  = refs.value.cView;
            if (cView == 0 || cView == LifeCycle::max_views ||
               (refs.value.cHandle == 0 && (cView == 1 || (cView == 2 &&
                refs.value.nWeakReference == LifeCycle::inflated))))
                {
                _detachCaptured();
                }
            else
                {
                // inline non-escaped detach
                refs.value.cView = cView - 1;
                m_atomicLifeCycleRefs.poke(refs);
                }
            }

        /**
        * Return if the Object is currently marked as escaped.
        *
        * An escaped object is one which is known to be reachable by multiple
        * threads.
        *
        * @param fAttemptCapture  true if capture should be attempted
        *
        * @return true if the Object is currently marked as escaped
        */
        bool _isEscaped(bool fAttemptCapture = true) const
            {
            LifeCycle::State state(m_atomicLifeCycleState.peek());

            if (fAttemptCapture && state.value.fEscaped)
                {
                LifeCycle::Refs refs(m_atomicLifeCycleRefs.get()); // must do a volatile read
                if (refs.value.cHandle + refs.value.cView == 1)
                    {
                    // there is only one ref to this object, and this thread has a ref
                    // therefore it is safe to capture the object
                    _capture();
                    return false;
                    }
                }

            return state.value.fEscaped;
            }


    // ----- life cycle events ----------------------------------------------

    protected:
        /**
        * Event called once the Object has finished being constructed.
        * Specifically when the first attachment is made. This provides a
        * safe point at which Handles/Views to "this" can be created. It is
        * not safe to create Handles/Views to an Object from within its
        * constructor, thus any operations which rely upon this should be
        * deferred until the onInit event is triggered.
        *
        * As with all event methods any derived implementation should
        * include a call to the super class's implementation. Specifically
        * delegation to Object::onInit() must eventually occur or an
        * IllegalStateException will result.
        *
        * The default implementation calls the onInit() method of each of the
        * Object's SmartMembers.
        */
        virtual void onInit();

        /**
        * Event called when the Object becomes only referenced via const
        * pointers (Views). Assuming a const-correct class, once this method
        * returns no further visible changes can be made to the object.
        *
        * As with all event methods any derived implementation should include
        * a call to the super class's implementation.
        *
        * The default implementation calls the onConst() method of each of
        * the Object's SmartMembers.
        *
        * @see isImmutable
        */
        virtual void onConst();

        /**
        * Event called when the guarding Object's escape state changes.
        *
        * As with all event methods any derived implementation should include
        * a call to the super class's implementation.  Ultimately delegation
        * must reach Object::onEscape() which will perform the actual act of
        * preparing the object for multi/single-threaded access.
        *
        * Throughout the call it is guaranteed that the object remains visible
        * to only a single thread, and as such it is not allowable to perform
        * an action from within this method which would attempt to escape this
        * object.
        *
        * @param fEscaped  true if the object is escaping, false if it is
        *                  being captured
        */
        virtual void onEscape(bool fEscaped) const;


    // ----- helper methods -------------------------------------------------

    protected:
        /**
        * Return a reference to the constructed base Object.
        *
        * This reference is suitable for use in derived class constructors as
        * the Object class will have completed construction.
        *
        * @return the Object reference
        */
        Object& self()
            {
            return *this;
            }

        /**
        * Return a reference to the constructed base Object.
        *
        * @return the const Object reference
        */
        const Object& self() const
            {
            return *this;
            }

    private:
        /**
        * Destroy the object.
        */
        void _destroy() const;

        /**
        * Escape an Object
        */
        void _escape() const;

        /**
        * Capture an escaped object.
        *
        * Note it is up to the caller to ensure that it is safe to perform the capture.  Specifically
        * the caller must have ensured that the object is currently marked as escaped, but that the
        * cumulative reference count is now only one, i.e. the only reference is held by the calling
        * thread.
        */
        void _capture() const;

        /**
        * Increment the Object's view reference count on a non-escaped object.
        *
        * @param fEscaped  true if the view will be escaped
        *
        * @return NULL on failed attach otherwise return this
        */
        const Object* _attachCaptured(bool fEscaped) const;

        /**
        * Increment the Object's handle reference count on an non-escaped object.
        *
        * @param fEscaped  true if the handle will be escaped
        *
        * @return NULL on failed attach otherwise return this
        */
        Object* _attachCaptured(bool fEscaped);

        /**
        * Increment the Object's view reference count on an escaped object.
        *
        * @param fEscaped  true if the view will be escaped
        *
        * @return NULL on failed attach otherwise return this
        */
        const Object* _attachEscaped(bool fEscaped) const;

        /**
        * Increment the Object's handle reference count on an escaped object.
        *
        * @param fEscaped  true if the view will be escaped
        *
        * @return NULL on failed attach otherwise return this
        */
        Object* _attachEscaped(bool fEscaped);

        /**
        * Decrement the Object's view reference count on a non-escaped object.
        */
        void _detachCaptured() const;

        /**
        * Decrement the Object's handle reference count on an non-escaped object.
        */
        void _detachCaptured();

        /**
        * Decrement the Object's view reference count on an escaped object.
        *
        * @param fEscaped  true if the view is escaped
        */
        void _detachEscaped(bool fEscaped) const;

        /**
        * Decrement the Object's handle reference count on an escaped object.
        *
        * @param fEscaped  true if the view is escaped
        */
        void _detachEscaped(bool fEscaped);

        /**
        * Acquire the shared read lock for the Object's data members.
        *
        * The data member read/write lock is intended for protecting small
        * sections of non-blocking code, such as primitive data-member access
        * and assignment. The read lock does support limited recursion,
        * and each call to acquire must be matched by a corresponding
        * call to release. For more advanced read/write lock support the
        * ThreadGate class should be utilized.
        *
        * The member read lock may be acquired using the
        * COH_SYNCHRONIZED_MEMBER_READ macro.
        *
        * @see SynchronizedMemberReadBlock
        * @see coherence::util::ThreadGate
        */
        void _acquireMemberReadLockInternal() const;

        /**
        * Release the read lock for the Object's data members
        */
        void _releaseMemberReadLockInternal() const;

        /**
        * Acquire the write lock for the Object's data members.
        *
        * Unlike the member read lock, write locks do not support
        * recursion, nor do they support lock promotion from a read to a
        * write lock. Attempts to use them such a manor will result in
        * dead-lock.
        *
        * The member write lock may be acquired using the
        * COH_SYNCHRONIZED_MEMBER_WRITE macro.
        *
        * @see SynchronizedMemberWriteBlockm
        */
        void _acquireMemberWriteLockInternal() const;

        /**
        * Release the write lock for the Object's data members.
        */
        void _releaseMemberWriteLockInternal() const;

        /**
        * Return the WeakReference associated with this Object, creating one
        * if necessary.
        *
        * @return the WeakReference associated with this Object
        */
        TypedHandle<WeakReference> _ensureWeakReference() const;

        /**
        * Return the NativeCondition which backs the Object's monitor,
        * creating one if necessary.
        *
        * The life of the returned condition is bounded to the Object.
        *
        * @return the NativeCondition which backs the Object's monitor
        */
        NativeCondition& _ensureMonitor() const;


    // ----- static methods -------------------------------------------------

    public:
        /**
        * Compare two Objects for equality.
        *
        * This method implements an <i>equivalence relation</i> for two
        * potentially <tt>NULL</tt> handles.
        *
        * @param v1  the first Object to compare
        * @param v2  the second Object to compare
        *
        * @return  <tt>true</tt> iff v1 and v2 are <tt>NULL</tt> or
        *          reference Objects which are equal
        */
        static bool equals(Object::View v1, Object::View v2);

        /**
        * Return the string form of the supplied Object
        *
        * If the handle is NULL then the string "NULL" will be written to the
        * stream.
        *
        * @param v the Object to output
        *
        * @return the object's toString
        */
        static TypedHandle<const String> toString(Object::View v);

        /**
        * Return a clone of the supplied Object.
        *
        * @param v  the Object to clone
        *
        * @return a clone of the Object, or NULL if NULL was supplied
        */
        static Object::Handle clone(Object::View v);

        /**
        * Return the hashCode for the specified object or 0 if the object is
        * NULL.
        *
        * @param v  the Object to hash
        *
        * @return the object's hash.
        */
        static size32_t hashCode(Object::View v)
            {
            return NULL == get_pointer(v) ? 0 : v->hashCode();
            }


    // ----- memory management ----------------------------------------------

    protected:
        /**
        * @internal
        *
        * Default allocator for Objects.
        *
        * @param cb  the number of bytes to allocate
        *
        * @return the allocated memory
        *
        * @throws IllegalStateException if the allocation fails
        */
        static void* operator new(size_t cb);

        /**
        * @internal
        *
        * Default deallocator for Objects.
        *
        * @param po  the memory to deallocate
        */
        static void operator delete(void* po);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * 32bit atomic Object life-cycle state.
        */
        mutable NativeAtomic32 m_atomicLifeCycleState;

        /**
        * 32bit atomic Object life-cycle refs.
        */
        mutable NativeAtomic32 m_atomicLifeCycleRefs;

        /**
        * Pointer to the NativeCondition which is used to implement the
        * Object's monitor functionality.
        *
        * The validity of this pointer is governed by the Object's
        * life-cycle. A non-NULL pointer is not sufficient to ensure that
        * the condition is ready for use.
        */
        mutable NativeCondition* m_pCondition;

        /**
        * The WeakReference to this Object.
        *
        * The validity of this handle is governed by the Object's life-cycle.
        * A non-NULL handle is not sufficient to ensure that the reference is
        * ready for use.
        */
        mutable WeakReference* m_pWeakThis;

        /**
        * Pointer to the top of the stack of SmartMembers bound to this
        * Object, or NULL if none exist.
        */
        mutable SmartMember* m_pSmartMemberStack;


    // ----- friends --------------------------------------------------------

    template<class> friend class Volatile;
    friend class SmartMember;
    friend class SynchronizedBlock;
    friend class SynchronizedMemberReadBlock;
    friend class SynchronizedMemberWriteBlock;
    friend class System;
    };


// ----- non-member operators and functions ---------------------------------

// Note: operator<< functions for Object are located in String.hpp as they need
// full String definition for their implementation. They also need to be fully
// inlineable and thus can't be split into the .cpp file either.

/**
* Object reference equality operator.
*/
template<class T>
inline bool operator==(const Object* cpo, const TypedHandle<T> &v)
    {
    return ((const Object*) get_pointer(v)) == cpo;
    }

/**
* Object reference equality operator.
*/
template<class T>
inline bool operator==(const TypedHandle<T> &v, const Object* cpo)
    {
    return ((const Object*) get_pointer(v)) == cpo;
    }

/**
* Object reference equality operator.
*/
template<class T1, class T2>
inline bool operator==(const TypedHandle<T1>& v1, const TypedHandle<T2>& v2)
    {
    return ((const Object*) get_pointer(v1)) ==
           ((const Object*) get_pointer(v2));
    }

/**
* Object reference inequality operator.
*/
template<class T>
inline bool operator!=(const Object* cpo, const TypedHandle<T>& v)
    {
    return ((const Object*) get_pointer(v)) != cpo;
    }

/**
* Object reference inequality operator.
*/
template<class T>
inline bool operator!=(const TypedHandle<T>& v, const Object* cpo)
    {
    return ((const Object*) get_pointer(v)) != cpo;
    }

/**
* Object reference inequality operator.
*/
template<class T1, class T2>
inline bool operator!=(const TypedHandle<T1>& v1, const TypedHandle<T2>& v2)
    {
    return ((const Object*) get_pointer(v1)) !=
           ((const Object*) get_pointer(v2));
    }

/**
* Object reference equality operator.
*/
inline bool operator==(const Object::View& v1, const Object::View& v2)
    {
    return ((const Object*) get_pointer(v1)) ==
           ((const Object*) get_pointer(v2));
    }

/**
* Object reference inequality operator.
*/
inline bool operator!=(const Object::View& v1, const Object::View& v2)
    {
    return ((const Object*) get_pointer(v1)) !=
           ((const Object*) get_pointer(v2));
    }

/**
* @internal
*
* Specialized extends implementation for classes deriving directly from Object.
*
* This specialization bring Object in virtually (as interfaces derive from it
* virtually as well.
*/
template<class A> class COH_EXPORT_SPEC extends<Object, A>
    : public virtual Object
    {
    public:
        typedef Void<Object> inherited;
        typedef A            alias;
        typedef Object       inherited_literal;
    };

/**
* @internal
*
* Specialized extends implementation for classes deriving directly from Object.
*
* This specialization bring Object in virtually (as interfaces derive from it
* virtually as well.
*/
template<> class COH_EXPORT_SPEC extends<Object, Void<Object> >
    : public virtual Object
    {
    public:
        typedef Void<Object> inherited;
        typedef Void<Object> alias;
        typedef Object       inherited_literal;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_OBJECT_HPP
