/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */

#include "coherence/tests/SimplePersonMapTrigger.hpp"

COH_OPEN_NAMESPACE2(coherence, tests)

using coherence::util::MapTrigger;

/**
* A MapTrigger for the SimplePerson class, implements a PortableObject.
*
* @author par  2011.07.14
* @since Coherence 12.1.3
*/

// ----- constructors ---------------------------------------------------

SimplePersonMapTrigger::SimplePersonMapTrigger()
    {
    }

// ----- MapTrigger interface -------------------------------------------

void SimplePersonMapTrigger::process(MapTrigger::Entry::Handle hEntry)
    {
    SimplePerson::Handle hPerson  = cast<SimplePerson::Handle>(hEntry->getValue());
    String::View vsName           = hPerson->getLastName();
    String::View vsNameUC         = XmlHelper::toUpperCase(vsName);

    if (!vsNameUC->equals(vsName))
        {
        hPerson->setLastName(vsNameUC);
        hEntry->setValue(hPerson);
        }
    }

// ----- PortableObject interface ---------------------------------------

void SimplePersonMapTrigger::readExternal(PofReader::Handle /* hIn */)
    {
    }

void SimplePersonMapTrigger::writeExternal(PofWriter::Handle /* hOut */) const
    {
    }

COH_REGISTER_PORTABLE_CLASS(3009, SimplePersonMapTrigger);

COH_CLOSE_NAMESPACE2

