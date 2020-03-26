/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/OutOfMemoryError.hpp"

#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/System.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return a pre-allocated OOME.
    */
    OutOfMemoryError::View getInstance()
        {
        static FinalView<OutOfMemoryError> vOOME(System::common(),
                OutOfMemoryError::create());
        static bool fInit = true;
        if (fInit)
            {
            fInit = false;
            // burn in empty stack
            vOOME->getDescription();
            }
        return vOOME;
        }
    COH_STATIC_INIT(getInstance());
    }

// ----- constructor --------------------------------------------------------

OutOfMemoryError::OutOfMemoryError(String::View vsMsg, Exception::View vCause)
    : super(vsMsg, vCause)
    {
    }


// ----- static helpers -----------------------------------------------------

void OutOfMemoryError::ensure(void* p, size_t cb)
    {
    if (NULL == p)
        {
        if (cb && cb <= sizeof(OutOfMemoryError))
            {
            // To avoid endless recursion we throw a pre-allocated OOME if the
            // failed size is <= to the size of OOME.
            COH_THROW (getInstance());
            }
        else if (cb == 0)
            {
            COH_THROW (OutOfMemoryError::create());
            }
        else
            {
            COH_THROW_STREAM (OutOfMemoryError,
                    "failure to allocate " << cb << " bytes");
            }
        }
    }

COH_CLOSE_NAMESPACE2
