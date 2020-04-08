/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/ThreadGroup.hpp"

#include "coherence/lang/NoSuchElementException.hpp"
#include "coherence/lang/Thread.hpp"
#include "coherence/util/ConcurrentModificationException.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Iterator.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

using coherence::util::ConcurrentModificationException;
using coherence::util::HashSet;
using coherence::util::Iterator;
using coherence::util::Set;

// ----- constructors ---------------------------------------------------

ThreadGroup::ThreadGroup(String::View vsName, ThreadGroup::Handle hParent)
    : f_vsName(self(), vsName), m_hGroupParent(self()), m_fDestroyed(false),
      f_hSetThreads(self(), HashSet::create()),
      f_hSetGroups(self(), HashSet::create())
    {
    static bool fFirst = true;
    if (NULL == hParent)
        {
        if (fFirst)
            {
            // only the first group is the only one allowed to not have
            // a parent
            fFirst = false;
            }
        else
            {
            hParent = Thread::currentThread()->getThreadGroup();
            }
        }

    if (NULL != hParent)
        {
        m_hGroupParent = hParent;
        hParent->addGroup(this);
        }
    }


// ----- ThreadGroup interface ------------------------------------------

String::View ThreadGroup::getName() const
    {
    return f_vsName;
    }

ThreadGroup::View ThreadGroup::getParent() const
    {
    return m_hGroupParent;
    }

void ThreadGroup::destroy()
    {
    ThreadGroup::Handle hParent = m_hGroupParent;

    COH_ENSURE(activeCount() == 0); // active
    COH_ENSURE(hParent != NULL);    // root or already destroyed

    for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->iterator();
            hIter->hasNext(); )
        {
        cast<ThreadGroup::Handle>(hIter->next())->destroy();
        }

    hParent->removeGroup(this);
    m_fDestroyed   = true;
    m_hGroupParent = NULL;
    }

bool ThreadGroup::isDestroyed() const
    {
    return m_fDestroyed;
    }

size32_t ThreadGroup::activeCount(bool fRecurse) const
    {
    size32_t cThreads = cast<Set::View>(f_hSetThreads)->size();
    if (fRecurse)
        {
        for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->iterator();
                hIter->hasNext(); )
            {
            cThreads += cast<ThreadGroup::View>(hIter->next())->activeCount(true);
            }
        }

    return cThreads;
    }

size32_t ThreadGroup::activeGroupCount(bool fRecurse) const
    {
    size32_t cGroups = cast<Set::View>(f_hSetGroups)->size();
    if (fRecurse)
        {
        for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->iterator();
                hIter->hasNext(); )
            {
            cGroups += cast<ThreadGroup::View>(hIter->next())->activeGroupCount(true);
            }
        }

    return cGroups;
    }

size32_t ThreadGroup::enumerateThreads(ObjectArray::Handle hao, bool fRecurse)
    {
    while (true)
        {
        try
            {
            size32_t cThreads = 0;
            size32_t cLimit   = hao->length;
            for (Iterator::Handle hIter = cast<Set::View>(f_hSetThreads)->
                    iterator(); hIter->hasNext(); )
                {
                if (cThreads == cLimit)
                    {
                    return cThreads;
                    }
                hao[cThreads++] = hIter->next();
                }

            if (fRecurse)
                {
                for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                        iterator(); hIter->hasNext(); )
                    {
                    if (cThreads == cLimit)
                        {
                        return cThreads;
                        }
                    cThreads += cast<ThreadGroup::Handle>(hIter->next())->
                            enumerateThreads(hao->subArray(cThreads, cLimit));
                    }
                }

            return cThreads;
            }
        catch (ConcurrentModificationException::View vex) {}
        catch (NoSuchElementException::View vex) {}
        }
    }

size32_t ThreadGroup::enumerateThreads(ObjectArray::Handle hao, bool fRecurse) const
    {
    while (true)
        {
        try
            {
            size32_t cThreads = 0;
            size32_t cLimit   = hao->length;

            for (Iterator::Handle hIter = cast<Set::View>(f_hSetThreads)->
                    iterator(); hIter->hasNext(); )
                {
                if (cThreads == cLimit)
                    {
                    return cThreads;
                    }
                hao[cThreads++] = (Object::View) hIter->next();
                }

            if (fRecurse)
                {
                for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                        iterator(); hIter->hasNext(); )
                    {
                    if (cThreads == cLimit)
                        {
                        return cThreads;
                        }
                    cThreads += cast<ThreadGroup::View>(hIter->next())->
                            enumerateThreads(hao->subArray(cThreads, cLimit));
                    }
                }

            return cThreads;
            }
        catch (ConcurrentModificationException::View vex) {}
        catch (NoSuchElementException::View vex) {}
        }
    }


size32_t ThreadGroup::enumerateGroups(ObjectArray::Handle hao, bool fRecurse)
    {
    while (true)
        {
        try
            {
            size32_t cGroups = 0;
            size32_t cLimit  = hao->length;

            for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                    iterator(); hIter->hasNext(); )
                {
                if (cGroups == cLimit)
                    {
                    return cGroups;
                    }
                hao[cGroups++] = hIter->next();
                }

            if (fRecurse)
                {
                for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                        iterator(); hIter->hasNext(); )
                    {
                    if (cGroups == cLimit)
                        {
                        return cGroups;
                        }
                    cGroups += cast<ThreadGroup::Handle>(hIter->next())->
                            enumerateGroups(hao->subArray(cGroups, cLimit));
                    }
                }

            return cGroups;
            }
        catch (ConcurrentModificationException::View vew) {}
        catch (NoSuchElementException::View vex) {}
        }
    }

size32_t ThreadGroup::enumerateGroups(ObjectArray::Handle hao, bool fRecurse) const
    {
    while (true)
        {
        try
            {
            size32_t cGroups = 0;
            size32_t cLimit  = hao->length;

            for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                    iterator(); hIter->hasNext(); )
                {
                if (cGroups == cLimit)
                    {
                    return cGroups;
                    }
                hao[cGroups++] = (Object::View) hIter->next();
                }

            if (fRecurse)
                {
                for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                        iterator(); hIter->hasNext(); )
                    {
                    if (cGroups == cLimit)
                        {
                        return cGroups;
                        }
                    cGroups += cast<ThreadGroup::View>(hIter->next())->
                            enumerateGroups(hao->subArray(cGroups, cLimit));
                    }
                }

            return cGroups;
            }
        catch (ConcurrentModificationException::View vex) {}
        catch (NoSuchElementException::View vex) {}
        }
    }


void ThreadGroup::interrupt(bool fRecurse)
    {
    while (true)
        {
        try
            {
            for (Iterator::Handle hIter = cast<Set::View>(f_hSetThreads)->
                    iterator(); hIter->hasNext(); )
                {
                cast<Thread::Handle>(hIter->next())->interrupt();
                }

            if (fRecurse)
                {
                for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                        iterator(); hIter->hasNext(); )
                    {
                    cast<ThreadGroup::Handle>(hIter->next())->interrupt();
                    }
                }
            return;
            }
        catch (ConcurrentModificationException::View vex) {}
        catch (NoSuchElementException::View vex) {}
        }
    }

size32_t ThreadGroup::join(int64_t cMillis, bool fRecurse) const
    {
    int64_t ldtEnd = cMillis == 0 ? 0 : System::currentTimeMillis() + cMillis;

    while (true)
        {
        try
            {
            for (Iterator::Handle hIter = cast<Set::View>(f_hSetThreads)->iterator();
                    hIter->hasNext(); )
                {
                if (cMillis == 0)
                    {
                    cast<Thread::Handle>(hIter->next())->join();
                    }
                else
                    {
                    int64_t cMillisWait = ldtEnd - System::currentTimeMillis();
                    if (cMillisWait <= 0)
                        {
                        return activeCount(fRecurse);
                        }
                    cast<Thread::Handle>(hIter->next())->join(cMillisWait);
                    }
                }

            if (fRecurse)
                {
                for (Iterator::Handle hIter = cast<Set::View>(f_hSetGroups)->
                        iterator(); hIter->hasNext(); )
                    {
                    if (cMillis == 0)
                        {
                        cast<ThreadGroup::View>(hIter->next())->join();
                        }
                    else
                        {
                        int64_t cMillisWait = ldtEnd - System::currentTimeMillis();
                        if (cMillisWait <= 0)
                            {
                            return activeCount(fRecurse);
                            }
                        cast<ThreadGroup::View>(hIter->next())->join(cMillisWait);
                        }
                    }
                }

            return 0;
            }
        catch (ConcurrentModificationException::View vex) {}
        catch (NoSuchElementException::View vex) {}
        }
    }

void ThreadGroup::add(Thread::Handle hThread)
    {
    COH_ENSURE(!isDestroyed());
    cast<Set::Handle>(f_hSetThreads)->add(hThread);
    }

void ThreadGroup::remove(Thread::Handle hThread)
    {
    cast<Set::Handle>(f_hSetThreads)->remove(hThread);
    }

void ThreadGroup::addGroup(ThreadGroup::Handle hGroup)
    {
    COH_ENSURE(!isDestroyed());
    cast<Set::Handle>(f_hSetGroups)->add(hGroup);
    }

void ThreadGroup::removeGroup(ThreadGroup::Handle hGroup)
    {
    cast<Set::Handle>(f_hSetGroups)->remove(hGroup);
    }


// ----- Object interface -----------------------------------------------

TypedHandle<const String> ThreadGroup::toString() const
    {
    String::View vs = COH_TO_STRING("ThreadGroup: " << getName() << ' ' << f_hSetThreads);
    Set::View vSetGroups = cast<Set::View>(f_hSetGroups);
    if (!vSetGroups->isEmpty())
        {
        vs = COH_TO_STRING(vs << std::endl << "  SubGroups: " << vSetGroups);
        }

    return vs;
    }


COH_CLOSE_NAMESPACE2
