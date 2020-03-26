/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "private/coherence/io/pof/reflect/internal/NameManglers.hpp"

#include <cctype>

COH_OPEN_NAMESPACE5(coherence,io,pof,reflect,internal)

// ----- FieldMangler interface ---------------------------------------------

String::View NameManglers::FieldMangler::mangle(String::View vsName) const
    {
    if (NULL == vsName)
        {
        return vsName;
        }

    String::View vsMangled  = vsName;
    const char*  achMangled = vsMangled->getCString();

    if (vsMangled->startsWith("m_"))
        {
        // handle the case where we have a variable name of m_fFoo
        // which should evaluate the name to be foo
        size32_t iStart = islower(achMangled[2]) && isupper(achMangled[3]) ? 3 : 2;
        vsMangled = COH_TO_STRING((char) tolower(achMangled[iStart]) << achMangled + (iStart + 1));
        }
    else if (achMangled[0] == 'f' && isupper(achMangled[1]))
        {
        // if this is fBar then strip the f and lowercase the B
        vsMangled = COH_TO_STRING((char) tolower(achMangled[1]) << achMangled + 2);
        }
    return vsMangled;
    }

// ----- MethodMangler interface --------------------------------------------

String::View NameManglers::MethodMangler::mangle(String::View vsName) const
    {
    if (NULL == vsName)
        {
        return vsName;
        }

    String::View vsMangled  = vsName;
    const char*  achMangled = vsMangled->getCString();

    if (vsMangled->startsWith("get") || vsMangled->startsWith("set"))
        {
        // lowercase the char after get or set
        vsMangled = COH_TO_STRING((char) tolower(achMangled[3]) << achMangled + 4);
        }
    else if (vsMangled->startsWith("is"))
        {
        // lowercase the char after is
        vsMangled = COH_TO_STRING((char) tolower(achMangled[2]) << achMangled + 3);
        }
    return vsMangled;
    }

// ----- static members -----------------------------------------------------

NameMangler::View NameManglers::getFieldMangler()
    {
    static FinalView<NameManglers::FieldMangler> vFieldMangler(System::common(),
            NameManglers::FieldMangler::create());
    return vFieldMangler;
    }
COH_STATIC_INIT(NameManglers::getFieldMangler());

NameMangler::View NameManglers::getMethodMangler()
    {
    static FinalView<NameManglers::MethodMangler> vMethodMangler(System::common(),
            NameManglers::MethodMangler::create());
    return vMethodMangler;
    }
COH_STATIC_INIT(NameManglers::getMethodMangler());

COH_CLOSE_NAMESPACE5
