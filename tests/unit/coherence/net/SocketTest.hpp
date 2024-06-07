/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "cxxtest/TestSuite.h"

#include "coherence/lang.ns"

#include "coherence/io/InterruptedIOException.hpp"
#include "coherence/net/InetSocketAddress.hpp"
#include "coherence/util/ArrayList.hpp"

#include "private/coherence/io/InputStream.hpp"
#include "private/coherence/io/OutputStream.hpp"

#include "private/coherence/net/ServerSocket.hpp"
#include "private/coherence/net/Socket.hpp"


#include <iostream>

using namespace coherence::lang;
using namespace coherence::io;
using namespace coherence::util;
using namespace coherence::net;
using namespace std;

/**
* Test helper class which echos socket input to output
*/
class EchoWorker
    : public class_spec<EchoWorker,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<EchoWorker>;

    protected:
        EchoWorker(Socket::Handle hSocket)
            : m_hSocket(self(), hSocket)
            {
            }

        void run()
            {
            Socket::Handle       hSocket = m_hSocket;
            InputStream::Handle  hInput  = hSocket->getInputStream();
            OutputStream::Handle hOutput = hSocket->getOutputStream();

            while (true)
                {
                try
                    {
                    hOutput->write(hInput->read());
                    }
                catch (InterruptedIOException::View vex)
                    {
                    hSocket->close();
                    return;
                    }
                }
            }

        FinalHandle<Socket> m_hSocket;
    };

/**
* Test helper class which accepts client connections and spawns EchoWorkers
*/
class EchoAcceptor
    : public class_spec<EchoAcceptor,
        extends<Object>,
        implements<Runnable> >
    {
    friend class factory<EchoAcceptor>;

    protected:
        EchoAcceptor(ServerSocket::Handle hSocket)
            : m_hSocket(self(), hSocket)
            {
            }

        void run()
            {
            ServerSocket::Handle hSocket      = m_hSocket;
            ArrayList
            ::Handle hListThreads = ArrayList::create();
            try
                {
                while (true)
                    {
                    // accept new connection, and spawn thread to handle it
                    Thread::Handle hWorker = Thread::create(EchoWorker::create(hSocket->accept()));
                    hWorker->start();
                    hListThreads->add(hWorker);
                    }
                }
            catch (InterruptedIOException::View)
                {
                // expected exception; shutdown workers
                for (Iterator::Handle hIter = hListThreads->iterator(); hIter->hasNext(); )
                    {
                    Thread::Handle hClient = cast<Thread::Handle>(hIter->next());
                    hClient->interrupt();
                    hClient->join();
                    }
                hSocket->close();
                }
                // any other exception is a failure
            }

        FinalHandle<ServerSocket> m_hSocket;
    };

/**
* SocketTest test suite
*/
class SocketTest : public CxxTest::TestSuite
    {
    public:
        /**
        * Basic end to end test.  Create a server socket, an acceptor, and
        * spawn echo threads.  Connect to the server, and validate the echo.
        * Then stop transmitting, timeout on read, interrupt acceptor, which
        * in turn interrupts echoers.  This test covers the vast majority of
        * the Socket related code.
        */
        void testEcho()
            {
            // setup and connect client and server
            InetSocketAddress::View vAddr   = InetSocketAddress::create("localhost", 18086);
            ServerSocket::Handle    hServer = ServerSocket::create();

            hServer->setReuseAddress(true);
            hServer->bind(vAddr);
            Thread::Handle hThreadServer = Thread::create(EchoAcceptor::create(hServer));
            Socket::Handle hClient       = Socket::create();
            hServer->setSoTimeout(10000); // test must complete within this interval
            hClient->setSoTimeout(10000);

            hThreadServer->start();
            hClient->connect(InetSocketAddress::create("localhost", 18086));

            // perform echo test

            OutputStream::Handle   hOutput   = hClient->getOutputStream();
            InputStream::Handle    hInput    = hClient->getInputStream();
            Array<octet_t>::Handle haEcho    = Array<octet_t>::create(11);
            Array<octet_t>::Handle haMessage = Array<octet_t>::create(11);

            haMessage[0]  = 'h';
            haMessage[1]  = 'e';
            haMessage[2]  = 'l';
            haMessage[3]  = 'l';
            haMessage[4]  = 'o';
            haMessage[5]  = ' ';
            haMessage[6]  = 'w';
            haMessage[7]  = 'o';
            haMessage[8]  = 'r';
            haMessage[9]  = 'l';
            haMessage[10] = 'd';

            // send the message
            hOutput->write(haMessage);
            hOutput->flush();

            // record echo from server
            hInput->readFully(haEcho);

            // validate that echo is correct
            TS_ASSERT(haEcho->equals(haMessage));

            // verify that no more data is on the socket
            int64_t ldtBlockStart = System::currentTimeMillis();
            try
                {
                hClient->setSoTimeout(10); // allow for quick timeout
                hInput->read();            // block here until timeout
                TS_ASSERT(false);          // should not reach here
                }
            catch (InterruptedIOException::View e)
                {
                int64_t cMillisBlocked = System::currentTimeMillis() - ldtBlockStart;
                TS_ASSERT(cMillisBlocked >= 5 && cMillisBlocked < 1000); // Note: Windows blocking sockets have minimum blocking time of 500ms!
                // expected timeout exception caught
                TS_ASSERT(e->getBytesTransfered() == 0);
                }
            // any other exception will fail test

            // test COH_TIMEOUT with socket
            ldtBlockStart = System::currentTimeMillis();
            COH_TIMEOUT_AFTER(1000)
                {
                try
                    {
                    hClient->setSoTimeout(5000); // will be overriden by timeout
                    hInput->read();            // block here until timeout
                    TS_ASSERT(false);          // should not reach here
                    }
                catch (InterruptedIOException::View e)
                    {
                    int64_t cMillisBlocked = System::currentTimeMillis() - ldtBlockStart;
                    TS_ASSERT(cMillisBlocked >= 900 && cMillisBlocked < 2000); // see note regarding Windows above
                    // expected timeout exception caught
                    TS_ASSERT(e->getBytesTransfered() == 0);
                    }
                // any other exception will fail test
                }

            // cleanup server and workers
            hThreadServer->interrupt();
            hThreadServer->join();

            // now that the socket is closed, read should throw an exception
            try
                {
                hInput->read();
                TS_ASSERT(false); // should not reach here
                }
            catch (IOException::View e)
                {
                }
            // any other exception will fail the test
            }
    };
