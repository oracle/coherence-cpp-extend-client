/*
 * Copyright (c) 2022, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include "coherence/io/OctetArrayReadBuffer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/io/pof/UserTypeReader.hpp"

#include "cxxtest/TestSuite.h"

using namespace coherence::lang;
using coherence::io::OctetArrayReadBuffer;
using coherence::io::pof::SystemPofContext;
using coherence::io::pof::UserTypeReader;
using namespace std;

/**
 * Test case for {@code COH-25103}.
 *
 * @author rl 5.3.22
 */
class Coh25103Test : public CxxTest::TestSuite
    {
    // ----- test lifecycle -------------------------------------------------

    public:
        void setUp()
            {
            m_hPofContext = SystemPofContext::getInstance();
            }

    // ----- tests ----------------------------------------------------------

    public:
        /**
         * Ensure it is possible to skip over a uniform Map containing
         * a {@code null} {@code T_CHAR_STRING} value.
         */
        void testCharString()
             {
             Array<octet_t>::Handle ahBytes = Array<octet_t>::create(9);
     
             ahBytes[0] = 0x01; // position 1
             ahBytes[1] = 0x5D; // t_uniform_map
             ahBytes[2] = 0x4E; // key-type   -> t_char_string
             ahBytes[3] = 0x4E; // value-type -> t_char_string
             ahBytes[4] = 0x01; // // one key/value pair
             ahBytes[5] = 0x01; // string length 1
             ahBytes[6] = 0x44; // letter 'D'
             ahBytes[7] = 0x64; // v_reference_null
             ahBytes[8] = 0x40; // EOS
     
             doReadRemainer(ahBytes);
             }

        /**
         * Ensure it is possible to skip over a uniform Map containing
         * a {@code null} {@code T_OCTET_STRING} value.
         */
        void testOctetString()
            {
            Array<octet_t>::Handle ahBytes = Array<octet_t>::create(9);

            ahBytes[0] = 0x01; // position 1
            ahBytes[1] = 0x5D; // t_uniform_map
            ahBytes[2] = 0x4C; // key-type   -> t_octet_string
            ahBytes[3] = 0x4C; // value-type -> t_octet_string
            ahBytes[4] = 0x01; // // one key/value pair
            ahBytes[5] = 0x01; // string length 1
            ahBytes[6] = 0x44; // letter 'D'
            ahBytes[7] = 0x64; // v_reference_null
            ahBytes[8] = 0x40; // EOS

            doReadRemainer(ahBytes);
            }

    // ----- helper methods -------------------------------------------------

    protected:
        void doReadRemainer(Array<octet_t>::Handle hBytes)
            {
            OctetArrayReadBuffer::Handle hReadBuffer = OctetArrayReadBuffer::create(hBytes, 0, hBytes->length, true);
            UserTypeReader::Handle       hReader     = UserTypeReader::create(hReadBuffer->getBufferInput(), m_hPofContext, 99999, 0);

            // ensure skipping doesn't raise an exception
            hReader->readRemainder();
            }

    // ----- data members ---------------------------------------------------

    protected:
        SystemPofContext::Handle m_hPofContext;
    };
