/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include <iostream>

using namespace coherence::lang;

Object::Handle pass_by_value(Object::Handle h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return h;
    }

Object::Handle pass_by_ref(const Object::Handle& h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return get_pointer(h);
    }

void pass_by_value_noret(Object::Handle h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return;
    }

void pass_by_ref_noret(const Object::Handle& h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return;
    }

Object::Handle pass_by_ref_ret(const Object::Handle& h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return h;
    }

void pass_by_ptr_noret(Object* p)
    {
    if (p == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return;
    }

Object::Handle pass_by_oldstyle (Object::Handle h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    return get_pointer(h);
    }

void pass_by_oldstyle_noret(Object::Handle h)
    {
    if (h == NULL)
        {
        std::cout << "shit" << std::endl;
        }
    }

