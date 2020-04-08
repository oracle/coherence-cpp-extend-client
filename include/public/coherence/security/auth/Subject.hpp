/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SUBJECT_HPP
#define COH_SUBJECT_HPP

#include "coherence/lang.ns"

#include "coherence/util/Set.hpp"
#include "coherence/security/RunAsBlock.hpp"
#include "coherence/security/Principal.hpp"

COH_OPEN_NAMESPACE3(coherence,security,auth)

using coherence::util::Set;

/**
* A Subject represents a grouping of related information for a single entity,
* such as a person.
*
* @see coherence::security::Principal
*
* @author jh 2007.12.20
*/
class COH_EXPORT Subject
    : public interface_spec<Subject>
    {
    // ----- Subject interface ----------------------------------------------

    public:
        /**
        * Return the set of Principals associated with the Subject.
        *
        * @return the set of Principals associated with the Subject
        */
        virtual Set::View getPrincipals() const = 0;
    };

COH_CLOSE_NAMESPACE3

#endif // COH_SUBJECT_HPP
