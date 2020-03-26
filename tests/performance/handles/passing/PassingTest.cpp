/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#include "coherence/lang.ns"

#include <iostream>

using namespace coherence::lang;

extern Object::Handle pass_by_value         (Object::Handle h);
extern Object::Handle pass_by_ref           (const Object::Handle& h);
extern void           pass_by_value_noret   (Object::Handle h);
extern void           pass_by_ref_noret     (const Object::Handle& h);
extern Object::Handle pass_by_ref_ret       (const Object::Handle& h);
extern void           pass_by_ptr_noret     (Object* p);
extern Object::Handle pass_by_oldstyle      (Object::Handle h);
extern void           pass_by_oldstyle_noret(Object::Handle h);

class PassingTest
    : public class_spec<PassingTest>
    {
    public:
        /**
        * Test entry point
        */
        static void main(ObjectArray::View vasArg)
            {
            int32_t cAssign = vasArg->length > 0
                ? Integer32::parse(cast<String::View>(vasArg[0]))
                : 10000;
            int32_t nStyle = vasArg->length > 1
                ? Integer32::parse(cast<String::View>(vasArg[1]))
                : 0;

            Object::Handle h = Boolean::create(true);
            Object*        p = get_pointer(h);

            std::cout << "h@ " << (void*)(&h) << std::endl;

            int64_t ldtStart = System::currentTimeMillis();

            for (int32_t i = 0; i < cAssign; ++i)
                {
                switch (nStyle)
                    {
                    case 0:
                        {
                        Object::Handle h2 = pass_by_value(h);
                        break;
                        }

                    case 1:
                        {
                        Object::Handle h2 = pass_by_ref(h);
                        break;
                        }

                    case 2:
                        {
                        pass_by_value_noret(h);
                        break;
                        }

                    case 3:
                        {
                        pass_by_ref_noret(h);
                        break;
                        }

                    case 4:
                        {
                        Object::Handle h2 = pass_by_ref_ret(h);
                        break;
                        }

                    case 5:
                        {
                        pass_by_ptr_noret(p);
                        break;
                        }

                    case 6:
                        {
                        Object::Handle h2 = pass_by_oldstyle(get_pointer(h));
                        break;
                        }

                    case 7:
                        {
                        pass_by_oldstyle_noret(get_pointer(h));
                        break;
                        }

                    default:
                        break;
                    }
                }

            int64_t ldtEnd = System::currentTimeMillis();
            int64_t cMillis = ldtEnd - ldtStart;

            std::cout << "performed " << cAssign
                << " assignments(" << nStyle << ") in "
                << cMillis << " ms; throughput = "
                << (cAssign / (cMillis / 1000.0)) << "/sec"
                << std::endl;
            }
    };
COH_REGISTER_EXECUTABLE_CLASS(PassingTest);
