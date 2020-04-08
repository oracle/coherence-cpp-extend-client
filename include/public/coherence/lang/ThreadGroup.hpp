/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_THREAD_GROUP_HPP
#define COH_THREAD_GROUP_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/TypedHandle.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)

// ----- forward declarations -----------------------------------------------

class Thread;


/**
* ThreadGroup represents a set of threads.
*
* @author mf 2009.11.17
*
* @since Coherence 3.6
*/
class COH_EXPORT ThreadGroup
    : public class_spec<ThreadGroup>
    {
    friend class factory<ThreadGroup>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ThreadGroup.
        *
        * @param vsName   the name for the group
        * @param hParent  the parent thread group, or NULL for current group
        */
        ThreadGroup(String::View vsName, ThreadGroup::Handle hParent = NULL);


    // ----- ThreadGroup interface ------------------------------------------

    public:
        /**
        * Return the name of the group.
        *
        * @return the group name
        */
        virtual String::View getName() const;

        /**
        * Return this groups parent group.
        *
        * @return the parent group
        */
        virtual ThreadGroup::View getParent() const;

        /**
        * Destroy this group and all of its subgroups. The groups must be
        * empty.
        */
        virtual void destroy();

        /**
        * Returns true if the group has been destroyed.
        *
        * @return true if the group has been destroyed.
        */
        virtual bool isDestroyed() const;

        /**
        * Return an estimate as to the number of threads in the group
        *
        * @param fRecurse  if the count should include sub-groups
        *
        * @return an estimate as to the number of threads in the group
        */
        virtual size32_t activeCount(bool fRecurse = true) const;

        /**
        * Return an estimate as to the number of thread groups in the group
        *
        * @param fRecurse  if the count should include sub-groups
        *
        * @return an estimate as to the number of thread groups in the group
        */
        virtual size32_t activeGroupCount(bool fRecurse = true) const;

        /**
        * Copy into the specified array the active threads represented by this
        * group.
        *
        * @param hao       the destination array
        * @param fRecurse  if sub-groups should be enumerated
        */
        virtual size32_t enumerateThreads(
                ObjectArray::Handle hao, bool fRecurse = true);

        /**
        * Copy into the specified array the active threads represented by this
        * group.
        *
        * @param hao       the destination array
        * @param fRecurse  if sub-groups should be enumerated
        */
        virtual size32_t enumerateThreads(
                ObjectArray::Handle hao, bool fRecurse = true) const;

        /**
        * Copy into the specified array the active thread groups represented
        * by this group.
        *
        * @param hao       the destination array
        * @param fRecurse  if sub-groups should be enumerated
        */
        virtual size32_t enumerateGroups(
                ObjectArray::Handle hao, bool fRecurse = true);

        /**
        * Copy into the specified array the active thread groups represented
        * by this group.
        *
        * @param hao       the destination array
        * @param fRecurse  if sub-groups should be enumerated
        */
        virtual size32_t enumerateGroups(
                ObjectArray::Handle hao, bool fRecurse = true) const;

        /**
        * Interrupt the threads represented by this group.
        *
        * @param fRecurse  if sub-groups should be enumerated
        */
        virtual void interrupt(bool fRecurse = true);

        /**
        * Join the threads represented by this group.
        *
        * @param cMillis   the total join timeout, or zero for infinite
        * @param fRecurse  if sub-groups should be enumerated
        *
        * @return the number of threads which failed to join within the timeout
        */
        virtual size32_t join(int64_t cMillis = 0, bool fRecurse = true) const;

    protected:
        /**
        * Add a thread to this group.
        *
        * @param hThread  the thread to add
        */
        virtual void add(TypedHandle<Thread> hThread);

        /**
        * Remove a thread from this group.
        *
        * @param hThread  the thread to remove
         */
        virtual void remove(TypedHandle<Thread> hThread);

        /**
        * Add a thread group to this group.
        *
        * @param hGroup  the group to add
        */
        virtual void addGroup(ThreadGroup::Handle hGroup);

        /**
        * Remove a thread group from this group.
        *
        * @param hGroup  the group to remove
        */
        virtual void removeGroup(ThreadGroup::Handle hGroup);


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The group name.
        */
        FinalView<String> f_vsName;

        /**
        * The parent group.
        */
        MemberHandle<ThreadGroup> m_hGroupParent;

        /**
        * True if the group has been destroyed.
        */
        bool m_fDestroyed;

        /**
        * The group's threads.
        */
        FinalHandle<Object> f_hSetThreads;

        /**
        * The group's child groups.
        */
        FinalHandle<Object> f_hSetGroups;


    // ----- friends --------------------------------------------------------

    friend class Thread;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_THREAD_GROUP_HPP
