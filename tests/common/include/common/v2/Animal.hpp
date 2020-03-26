/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_V2_ANIMAL_HPP
#define COH_TEST_V2_ANIMAL_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/SimpleEvolvable.hpp"

#include "coherence/io/pof/EvolvableHolder.hpp"
#include "coherence/io/pof/EvolvableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"
#include "coherence/io/pof/PortableObject.hpp"

COH_OPEN_NAMESPACE3(common, test, v2)

using namespace coherence::lang;

using coherence::io::Evolvable;
using coherence::io::SimpleEvolvable;
using coherence::io::pof::EvolvableHolder;
using coherence::io::pof::EvolvableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;
using coherence::io::pof::PortableObject;

class Animal
    : public class_spec<Animal,
        extends<Object>,
        implements<PortableObject, EvolvableObject> >
    {
    friend class factory<Animal>;

    // ----- constructors ---------------------------------------------------

    protected:
        Animal()
            : f_hEvolvable(self(), SimpleEvolvable::create(1)),
              f_hEvolvableHolder(self(), EvolvableHolder::create()),
              f_vsSpecies(self())
            {
            }

        Animal(String::View vSpecies)
            : f_hEvolvable(self(), SimpleEvolvable::create(1)),
              f_hEvolvableHolder(self(), EvolvableHolder::create()),
              f_vsSpecies(self(), vSpecies)
            {
            }

    // ----- Animal interface -----------------------------------------------

    public:
        virtual String::View getSpecies() const
            {
            return f_vsSpecies;
            }

        virtual void setSpecies(String::View vSpecies)
            {
            initialize(f_vsSpecies, vSpecies);
            }

    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            if (hIn->getUserTypeId() == s_nTypeId)
                {
                initialize(f_vsSpecies, hIn->readString(0));
                }
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            if (hOut->getUserTypeId() == s_nTypeId)
                {
                hOut->writeString(0, f_vsSpecies);
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

    // ----- Object overrides -----------------------------------------------

    public:
        virtual bool equals(Object::View v) const
            {
            return this == v || (Class::getClassName(this)->equals(Class::getClassName(v)) && matches(v));
            }

        virtual bool matches(Object::View v) const
            {
            if (instanceof<Animal::View>(v))
                {
                return f_vsSpecies->equals(cast<Animal::View>(v)->f_vsSpecies);
                }
            return false;
            }

        virtual size32_t hashCode() const
            {
            return f_vsSpecies->hashCode();
            }

        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("Animal.v2{" <<
                   "species='" << f_vsSpecies << '\'' <<
                   '}');
            }

    // ----- static constants -----------------------------------------------

    public:
        static const int32_t s_nTypeId = 51719;

    // ----- data members ---------------------------------------------------

    private:
        FinalHandle<Evolvable> f_hEvolvable;
        FinalHandle<EvolvableHolder> f_hEvolvableHolder;

    protected:
        FinalView<String> f_vsSpecies;
    };

const int32_t Animal::s_nTypeId;

COH_REGISTER_PORTABLE_TYPE_CLASS(51719, Animal);

COH_CLOSE_NAMESPACE3

#endif // COH_TEST_V2_ANIMAL_HPP
