/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/SubjectPofSerializer.hpp"

#include "coherence/security/auth/GenericSubject.hpp"
#include "coherence/security/auth/Subject.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::security::auth::GenericSubject;
using coherence::security::auth::Subject;
using coherence::util::HashSet;
using coherence::util::Set;


// ----- construtors --------------------------------------------------------

SubjectPofSerializer::SubjectPofSerializer()
    {
    }


// ----- PofSerializer interface ----------------------------------------

void SubjectPofSerializer::serialize(PofWriter::Handle hOut, Object::View v) const
    {
    Subject::View vSubject = cast<Subject::View>(v);

    hOut->writeCollection(0, vSubject->getPrincipals());
    hOut->writeRemainder(NULL);
    }

Object::Holder SubjectPofSerializer::deserialize(PofReader::Handle hIn) const
    {
    Set::View      vSet = cast<Set::View>(hIn->readCollection(0, HashSet::create()));
    Object::Holder oh   = GenericSubject::create(vSet);
    hIn->registerIdentity(oh);
    hIn->readRemainder();

    return oh;
    }

COH_CLOSE_NAMESPACE3
