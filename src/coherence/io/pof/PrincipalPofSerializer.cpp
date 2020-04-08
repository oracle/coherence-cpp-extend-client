/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/io/pof/PrincipalPofSerializer.hpp"

#include "coherence/io/pof/SystemPofContext.hpp"
#include "coherence/security/GenericPrincipal.hpp"
#include "coherence/security/Principal.hpp"
#include "coherence/util/HashSet.hpp"
#include "coherence/util/Set.hpp"

COH_OPEN_NAMESPACE3(coherence,io,pof)

using coherence::security::GenericPrincipal;
using coherence::security::Principal;
using coherence::util::HashSet;
using coherence::util::Set;


// ----- construtors --------------------------------------------------------

PrincipalPofSerializer::PrincipalPofSerializer()
    {
    }


// ----- PofSerializer interface ----------------------------------------

void PrincipalPofSerializer::serialize(PofWriter::Handle hOut, Object::View v) const
    {
    Principal::View vPrincipal = cast<Principal::View>(v);

    hOut->writeString(0, vPrincipal->getName());
    hOut->writeRemainder(NULL);
    }

Object::Holder PrincipalPofSerializer::deserialize(PofReader::Handle hIn) const
    {
    Object::Holder oh = GenericPrincipal::create(hIn->readString(0));
    hIn->registerIdentity(oh);
    hIn->readRemainder();

    return oh;
    }

COH_CLOSE_NAMESPACE3
