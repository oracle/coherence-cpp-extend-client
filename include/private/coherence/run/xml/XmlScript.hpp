/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_XML_SCRIPT_HPP
#define COH_XML_SCRIPT_HPP

#include "coherence/lang.ns"

#include "private/coherence/dev/compiler/SimpleScript.hpp"

COH_OPEN_NAMESPACE3(coherence,run,xml)

using coherence::dev::compiler::SimpleScript;


/**
* Implements a simple unicode script object for parsing Xml.
*
* @author tb  2007.12.13
*/
class COH_EXPORT XmlScript
    : public class_spec<XmlScript,
        extends<SimpleScript> >
    {
    friend class factory<XmlScript>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new XmlScript instance.
        */
        XmlScript();

        /**
        * Create a new XmlScript instance from a given string.
        *
        * @param vsScript  the string used to create the script
        */
        XmlScript(String::View vsScript);

    private:
        /**
        * Blocked copy constructor.
        */
        XmlScript(const XmlScript&);
    };

COH_CLOSE_NAMESPACE3

#endif // COH_XML_SCRIPT_HPP
