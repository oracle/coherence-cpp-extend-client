/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/security/RunAsBlock.hpp"

#include "coherence/security/auth/GenericSubject.hpp"
#include "coherence/util/Set.hpp"

#include "private/coherence/security/SecurityHelper.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

using coherence::security::auth::GenericSubject;
using coherence::util::Set;


// ----- file local helpers -------------------------------------------------

namespace
    {
    /**
    * Return a marker Subject used to indicate, that the RunAsBlock's
    * destructor should not push the Subject back.
    */
    Subject::View getMarkerSubject()
        {
        static FinalView<Subject> vSubjectMarker(System::common(),
            GenericSubject::create((Set::View) NULL));
        return vSubjectMarker;
        }
    COH_STATIC_INIT(getMarkerSubject());
    }

// ----- constructors -------------------------------------------------------

RunAsBlock::RunAsBlock(Subject::View vSubject)
    : m_vSubject(SecurityHelper::updateCurrentSubject(vSubject))
    {
    }

RunAsBlock::RunAsBlock(const RunAsBlock& that)
    : m_vSubject(that.m_vSubject)
    {
    that.m_vSubject = getMarkerSubject(); // take ownership
    }

RunAsBlock::~RunAsBlock()
    {
    Subject::View vSubject = m_vSubject;
    if (vSubject != getMarkerSubject())
        {
        SecurityHelper::updateCurrentSubject(vSubject);
        }
    }


// ----- operators ----------------------------------------------------------

RunAsBlock::operator bool() const
    {
    return false;
    }

COH_CLOSE_NAMESPACE2
