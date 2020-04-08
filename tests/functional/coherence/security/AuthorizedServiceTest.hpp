/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"
#include "coherence/net/AbstractInvocable.hpp"
#include "coherence/net/CacheFactory.hpp"
#include "coherence/net/CacheService.hpp"
#include "coherence/net/DefaultConfigurableCacheFactory.hpp"
#include "coherence/net/Invocable.hpp"
#include "coherence/net/InvocationService.hpp"
#include "coherence/net/Member.hpp"
#include "coherence/net/NamedCache.hpp"
#include "coherence/security/auth/GenericSubject.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/Map.hpp"

#include "private/coherence/component/net/extend/RemoteInvocationService.hpp"
#include "private/coherence/component/util/SafeInvocationService.hpp"
#include "private/coherence/net/messaging/Channel.hpp"
#include "private/coherence/net/messaging/Connection.hpp"
#include "private/coherence/net/messaging/ConnectionInitiator.hpp"

using namespace coherence::lang;

using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;
using coherence::net::AbstractInvocable;
using coherence::net::CacheFactory;
using coherence::net::CacheService;
using coherence::net::DefaultConfigurableCacheFactory;
using coherence::net::Invocable;
using coherence::net::InvocationService;
using coherence::net::Member;
using coherence::net::NamedCache;
using coherence::security::auth::GenericSubject;
using coherence::security::auth::Subject;
using coherence::util::Map;

using coherence::component::net::extend::RemoteInvocationService;
using coherence::component::util::SafeInvocationService;
using coherence::net::messaging::Channel;
using coherence::net::messaging::Connection;
using coherence::net::messaging::ConnectionInitiator;


class TestSubjectInvocable
    : public class_spec<TestSubjectInvocable,
        extends<AbstractInvocable>,
        implements<PortableObject> >
    {
    friend class factory<TestSubjectInvocable>;

    public:
        virtual void readExternal(PofReader::Handle /*hIn*/)
            {
            }

        virtual void writeExternal(PofWriter::Handle /*hOut*/) const
            {
            }
    };
COH_REGISTER_PORTABLE_CLASS(9999, TestSubjectInvocable);

/**
* AuthroizedService Test Suite.
*/
class AuthorizedServiceTest : public CxxTest::TestSuite
    {
    // ----- test fixtures ---------------------------------------------------

    public:
    /**
     * Called after each test.
     */
    void tearDown()
        {
        // hangs without the fix for COH-15840
        CacheFactory::shutdown();
        }

    // ----- tests -----------------------------------------------------------

    public:
        /**
        * Test Subject propagation to a service.
        */
        void testSubjectPropagationToService()
            {
            Subject::View vSubj = GenericSubject::create("Mr. Hacker");

            InvocationService::Handle hService;

            COH_RUN_AS(vSubj)
                {
                hService = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));
                }

            Member::View vMember =
                    cast<DefaultConfigurableCacheFactory::Handle>(
                            CacheFactory::getConfigurableCacheFactory())
                            ->ensureOperationalContext()->getLocalMember();

            TestSubjectInvocable::Handle hInvocable = TestSubjectInvocable::create();

            Map::View vMap = hService->query(hInvocable, NULL);
            TS_ASSERT(vSubj->equals(vMap->get(vMember)));

            SafeInvocationService::Handle hSafeService =
                    cast<SafeInvocationService::Handle>(hService);
            RemoteInvocationService::Handle hRemoteService =
                    cast<RemoteInvocationService::Handle>
                        (hSafeService->ensureRunningInvocationService(true));
            ConnectionInitiator::Handle hInitiator = hRemoteService->getInitiator();
            Connection::Handle hConnection = hInitiator->ensureConnection();
            TS_ASSERT(vSubj->equals(hConnection->getChannel(0)->getSubject()));
            }

        /**
        * Test Subject propagation to a cache. The test uses a wrapped cache
        * to authenticate calls. The wrapped cache also updates the
        * "current-subject" value in the map after each call so the test
        * client can assert the subject.
        */
        void testSubjectPropagationToCache()
            {
            try
                {
                Subject::View vSubj = GenericSubject::create("admin");
                String::View  vsKey               = "key";
                String::View  vsCurrentSubjectKey = "current-subject";
                String::View  vsVal               = "test-value";

                NamedCache::Handle hCache = NULL;


                // Execute within a COH_RUN_AS block which causes Subject
                // be propagated
                COH_RUN_AS(vSubj)
                  {
                  hCache = CacheFactory::getCache("secure-cache");
                  }

                hCache->put(vsKey, vsVal);

                TS_ASSERT(vsVal->equals(hCache->get(vsKey)));

                // Need to shutdown service to "reset" subject
                CacheService::Handle hService = hCache->getCacheService();
                hService->shutdown();
                }
            catch (Exception::View ve)
                {
                std::cerr << ve << std::endl;
                throw;
                }
            // Negative test case. Attempt to access without a Subject
            try
                {
      		    CacheFactory::getCache("secure-negative");
                TS_FAIL("Exception expected");
    		    }
    		catch (Exception::View ve)
    		    {
     		    // expected
    		    }
            }
        /**
        * Test Subject scoping for a cache. The test verifies that different
        * subjects get different cache references.
        */
        void testSubjectScopingForCache()
            {
            std::cerr << "Subject Scoped: "
                << cast<DefaultConfigurableCacheFactory::Handle>(
                        CacheFactory::getConfigurableCacheFactory())
                    ->ensureOperationalContext()->isSubjectScopingEnabled()
                << std::endl;
            Subject::View vSubjAdmin = GenericSubject::create("admin");
            Subject::View vSubjUser = GenericSubject::create("user");

            NamedCache::Handle hCacheAdmin = NULL;
            NamedCache::Handle hCacheUser = NULL;

            // Execute within a COH_RUN_AS block which causes Subject
            // be propagated
            COH_RUN_AS(vSubjAdmin)
              {
              hCacheAdmin = CacheFactory::getCache("dist-extend");
              }
            COH_RUN_AS(vSubjUser)
              {
              hCacheUser = CacheFactory::getCache("dist-extend");
              }

            TS_ASSERT(hCacheAdmin != hCacheUser);

            // Need to shutdown service to "reset" subject
            CacheService::Handle hService = hCacheAdmin->getCacheService();
            hService->shutdown();

            // Should return same reference when not using a subject
            NamedCache::Handle hCacheFirst =
                    CacheFactory::getCache("dist-extend");
            NamedCache::Handle hCacheSecond =
                    CacheFactory::getCache("dist-extend");

            TS_ASSERT(hCacheFirst == hCacheSecond);
            }

        /**
        * Test Subject scoping for the invocation service. The test verifies
        * that different subjects get different service references.
        */
        void testSubjectScopingForService()
            {
            Subject::View vSubjAdmin = GenericSubject::create("admin");
            Subject::View vSubjUser = GenericSubject::create("user");

            InvocationService::Handle hServiceAdmin;
            InvocationService::Handle hServiceUser;

            COH_RUN_AS(vSubjAdmin)
                {
                hServiceAdmin = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));
                }
            COH_RUN_AS(vSubjUser)
                {
                hServiceUser = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));
                }

            TS_ASSERT(hServiceAdmin != hServiceUser);

            // Need to shutdown service to "reset" subject
            hServiceAdmin->shutdown();
            hServiceUser->shutdown();

            InvocationService::Handle hServiceFirst;
            InvocationService::Handle hServiceSecond;

            // Should return same reference when not using a subject
            hServiceFirst = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));
            hServiceSecond = cast<InvocationService::Handle>
                    (CacheFactory::getService("ExtendTcpInvocationService"));

            TS_ASSERT(hServiceFirst == hServiceSecond);
            }
    };
