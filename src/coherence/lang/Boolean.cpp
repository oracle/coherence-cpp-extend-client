/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang/Boolean.hpp"

#include "coherence/lang/FinalHandle.hpp"
#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/System.hpp"
#include "coherence/lang/SystemClassLoader.hpp"
#include "coherence/lang/TypedBarrenClass.hpp"

COH_OPEN_NAMESPACE2(coherence,lang)

COH_REGISTER_CLASS(TypedBarrenClass<Boolean>::create());


// ----- constructors -------------------------------------------------------

Boolean::Boolean(bool f)
    : super(f)
    {
    }

Boolean::Boolean(const Boolean& that)
    : super(that)
    {
    }


// ----- static methods ----------------------------------------------------

Boolean::Handle Boolean::valueOf(bool f)
    {
    static FinalHandle<Boolean> hTrue(System::common(), create(true));
    static FinalHandle<Boolean> hFalse(System::common(), create(false));
    return f ? hTrue : hFalse;
    }
COH_STATIC_INIT(Boolean::valueOf(true));

String::View Boolean::toString(bool f)
    {
    static FinalView<String> vsTrue(System::common(), String::create("true"));
    static FinalView<String> vsFalse(System::common(), String::create("false"));
    return f ? vsTrue : vsFalse;
    }
COH_STATIC_INIT(Boolean::toString(true));

COH_CLOSE_NAMESPACE2
