/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TESTOUTPUT_HPP
#define COH_TESTOUTPUT_HPP

#include "coherence/lang.ns"

#include "private/coherence/util/logging/Standard.hpp"

#include <iostream>

using namespace coherence::lang;
using namespace coherence::util::logging;
using namespace std;

class TestOutput
    : public class_spec<TestOutput,
        extends<Standard> >
    {
    friend class factory<TestOutput>;

    protected:
        TestOutput(ostream& strm)
            {
            setOutput(strm);
            }
    public:

        inline void setOutput(ostream& strm)
            {
            m_Out = &strm;
            }
    };


#endif //COH_TESTOUTPUT_HPP


