/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/util/PagedMuterator.hpp"

COH_OPEN_NAMESPACE2(coherence,util)

// ----- constructor --------------------------------------------------------

PagedMuterator::PagedMuterator(Advancer::Handle hAdvancer)
    : super(hAdvancer)
    {
    }


// ----- Muterator interface ------------------------------------------------

void PagedMuterator::remove()
    {
    Object::Holder ohCurr = m_ohCurr;
    if (NULL == ohCurr)
        {
        COH_THROW (IllegalStateException::create());
        }

    struct InternalFinally
        {
        InternalFinally(PagedMuterator::Handle _hPagedMuterator)
                : hPagedMuterator(_hPagedMuterator)
            {
            }

        ~InternalFinally()
            {
            hPagedMuterator->m_ohCurr = NULL;
            }

        PagedMuterator::Handle hPagedMuterator;
        } finally(this);

        f_hAdvancer->remove(ohCurr);
    }

COH_CLOSE_NAMESPACE2
