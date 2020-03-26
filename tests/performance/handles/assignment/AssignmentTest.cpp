/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "coherence/lang.ns"
/*
#include "boost/shared_ptr.hpp"
#include "boost/thread/locks.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/recursive_mutex.hpp"
*/
#include "pthread.h"
#include <iostream>

using namespace coherence::lang;
//using namespace boost;
//using namespace boost::detail;
using coherence::native::NativeAtomic64;

class Dummy
    {
    public:
        Dummy() {};
    };

class Contender
    : public class_spec<Contender,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<Contender>;

    protected:
        Contender(Object::Handle hLock)
            : m_hLock(self(), hLock)
        {
        }

    public:
        virtual void run()
            {
            COH_SYNCHRONIZED (m_hLock)
                {

                }

            while (true)
                {
                Thread::sleep(10000);
                }
            }

        FinalHandle<Object> m_hLock;
    };

extern int64_t update64(int64_t& lAtomic, int64_t lAssume, int64_t lNew);

int64_t update64(int64_t& lAtomic, int64_t lAssume, int64_t lNew)
    {
    register int64_t lResult;

    // cmpxchg compares the destination operand (%1 - m_lAtomic) with
    // the values in RAX (a - lAssume). If they are equal, the
    // value of register "r" (lNew) is stored to the destination. The original
    // value is written to RAX(=a result).
    __asm__ __volatile__ (
        "lock; cmpxchg %3, %1"  // cas(atomic(%1), assumed(RAX), new(%3))
        : "=a" (lResult)    // %0 - output RAX            in RAX
        : "m"  (lAtomic),   // %1 - input atomic variable in memory
          "a"  (lAssume),   // %2 - input assumed value   in RAX
          "r"  (lNew)       // %3 - input new value       in any register
        : "cc",             // clobbered condition code register
          "memory");        // clobbered memory in unpredictable way

    return lResult;
    }

class AssignmentTest
    : public class_spec<AssignmentTest>
    {
    friend class factory<AssignmentTest>;

    public:
        /**
        * Test entry point
        */
        static void main(ObjectArray::View vasArg)
            {
            int32_t cAssign = vasArg->length > 0
                ? Integer32::parse(cast<String::View>(vasArg[0]))
                : 10000;
            int32_t nStyle = vasArg->length > 1
                ? Integer32::parse(cast<String::View>(vasArg[1]))
                : 0;

            Object::Handle       hCapture = Object::create();
            Object::Handle       h = Boolean::create(true);
            FinalHandle<Object>  hf(*hCapture, h);
            MemberHolder<Object> hm(*hCapture, h);
            Object*              p = get_pointer(h);
            const Object::Handle hc = h;
            Dummy* pDummy = new Dummy();

      /*      const shared_ptr<Dummy> cpDummy(pDummy);
            shared_ptr<Dummy> spDummy(cpDummy);
            mutex lock;
            recursive_mutex reclock;
*/
            pthread_mutexattr_t pt_attr;
            pthread_mutex_t pt_mutex;

            COH_ENSURE_EQUALITY(pthread_mutexattr_init(&pt_attr), 0);
            COH_ENSURE_EQUALITY(pthread_mutexattr_settype(&pt_attr, PTHREAD_MUTEX_RECURSIVE), 0);
            COH_ENSURE_EQUALITY(pthread_mutex_init(&pt_mutex, &pt_attr), 0);


            NativeAtomic64 natl;

            int64_t lAtomic = 0;

            int gatl;

/*            Thread::create(Contender::create(h))->start();

            Thread::sleep(2000);
*/

            int64_t ldtStart = System::currentTimeMillis();

            for (int32_t i = 0; i < cAssign; ++i)
                {
                switch (nStyle)
                    {
                    case 0:
                        {
                        Object::Handle h2 = h;
                        break;
                        }

                    case 1:
                        {
                        Object::Handle h2 = get_pointer(h);
                        break;
                        }

                    case 2:
                        {
                        p->_attach(/*fEscaped*/ false);
                        p->_detach(/*fEscaped*/ false);
                        break;
                        }

                    case 3:
                        {
                        Object::Holder oh(h);
                        break;
                        }

                    case 4:
                        {
                        Object::Handle h = hf;
                        break;
                        }

                    case 5:
                        {
                        Object::Holder h = hm;
                        break;
                        }

                    case 6:
                        {
                        Object::Holder h = p;
                        break;
                        }

                    case 7:
                        {
                        Object::Handle h = hc;
                        break;
                        }

                    case 8:
                        {
                        const Object::Handle h2 = h;
                        break;
                        }

                    case 9:
                        {
                        FinalView<Object> f(System::common(), h);
                        break;
                        }
/*
                    case 10:
                        {
                        shared_ptr<Dummy> spTmp(spDummy);
                        break;
                        }

                    case 11:
                        {
                        lock_guard<mutex> guard(lock);
                        shared_ptr<Dummy> spTmp(spDummy);
                        break;
                        }

                    case 12:
                        {
                        shared_ptr<Dummy> spTmp(cpDummy);
                        break;
                        }

                    case 13:
                        {
                        lock_guard<mutex> guard(lock);
//                        shared_ptr<Dummy> spTmp(cpDummy);
                        break;
                        }
*/
                    case 14:
                        {
                        COH_SYNCHRONIZED(h)
                            {
                            break;
                            }
                        }

                    case 15:
                        {
                        COH_SYNCHRONIZED(p)
                            {
                            break;
                            }
                        }

                    case 16:
                        {
                        natl.postAdjust(1, false);
                        break;
                        }
/*
                    case 17:
                        {
                        atomic_increment(&gatl);
                        break;
                        }
*/
                    case 18:
                        {
                        pthread_mutex_lock(&pt_mutex);
                        pthread_mutex_unlock(&pt_mutex);
                        break;
                        }

                    case 19:
                        {
                        hf->hashCode();
                        break;
                        }

                    case 20:
                        {
                        hm->hashCode();
                        break;
                        }

                    case 21:
                        {
                        h->hashCode();
                        break;
                        }

                    case 22:
                        {
                        p->hashCode();
                        break;
                        }

                    case 23:
                        {
                        update64(lAtomic, lAtomic, lAtomic + 1);
                        break;
                        }

                    default:
                        break;
                    }
                }

            int64_t ldtEnd = System::currentTimeMillis();
            int64_t cMillis = ldtEnd - ldtStart;

            std::cout << "performed " << cAssign
                << " assignments(" << nStyle << ") in "
                << cMillis << " ms; throughput = "
                << (cAssign / (cMillis / 1000.0)) << "/sec"
                << std::endl;
            }
    };
COH_REGISTER_EXECUTABLE_CLASS(AssignmentTest);
