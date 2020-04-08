/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_V1_PET_HPP
#define COH_TEST_V1_PET_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/SimpleEvolvable.hpp"

#include "coherence/io/pof/EvolvableHolder.hpp"
#include "coherence/io/pof/EvolvableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE3(common, test, v1)

using namespace coherence::lang;

using coherence::io::Evolvable;
using coherence::io::SimpleEvolvable;
using coherence::io::pof::EvolvableHolder;
using coherence::io::pof::EvolvableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

class Pet
    : public class_spec<Pet,
        extends<Object>,
        implements<PortableObject, EvolvableObject> >
    {
    friend class factory<Pet>;

    // ----- constructors ---------------------------------------------------

    protected:
        Pet()
            : f_vsName(self()),
              f_hEvolvableHolder(self(), EvolvableHolder::create()),
              f_hEvolvable(self(), SimpleEvolvable::create(1))
            {
            }

        Pet(String::View vsName)
            : f_vsName(self(), vsName),
              f_hEvolvableHolder(self(), EvolvableHolder::create()),
              f_hEvolvable(self(), SimpleEvolvable::create(1))
            {
            }

    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            if (hIn->getUserTypeId() == s_nTypeId)
                {
                initialize(f_vsName, hIn->readString(0));
                }
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            if (hOut->getUserTypeId() == s_nTypeId)
                {
                hOut->writeString(0, f_vsName);
                }
            }

    // ----- EvolvableObject interface --------------------------------------

    public:
        virtual Evolvable::View getEvolvable(int32_t nTypeId) const
            {
            if (nTypeId == s_nTypeId)
                {
                return f_hEvolvable;
                }
            return getEvolvableHolder()->get(nTypeId);
            }

        virtual Evolvable::Handle getEvolvable(int32_t nTypeId)
            {
            if (nTypeId == s_nTypeId)
                {
                return f_hEvolvable;
                }
            return getEvolvableHolder()->get(nTypeId);
            }

        virtual EvolvableHolder::View getEvolvableHolder() const
            {
            return f_hEvolvableHolder;
            }

    // ----- Pet methods ----------------------------------------------------

    public:
        virtual bool equals(Object::View v) const
            {
            return this == v || (Class::getClassName(this)->equals(Class::getClassName(v)) && matches(v));
            }

        virtual bool matches(Object::View v) const
            {
            if (instanceof<Pet::View>(v))
                {
                return f_vsName->equals(cast<Pet::View>(v)->f_vsName);
                }
            return false;
            }

    // ----- Object overrides -----------------------------------------------

    public:
        virtual size32_t hashCode() const
            {
            return f_vsName->hashCode();
            }

        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("Pet.v1{" <<
                   "name='" << f_vsName << '\''
                   << '}');
            }


    // ----- static constants -----------------------------------------------

    public:
        static const int32_t s_nTypeId = 51717;

    // ----- data members ---------------------------------------------------

    protected:
        FinalView<String> f_vsName;

    private:
        FinalHandle<EvolvableHolder> f_hEvolvableHolder;
        FinalHandle<Evolvable> f_hEvolvable;
    };

const int32_t Pet::s_nTypeId;

COH_REGISTER_TYPED_CLASS(Pet);

COH_CLOSE_NAMESPACE3

#endif // COH_TEST_V1_PET_HPP
