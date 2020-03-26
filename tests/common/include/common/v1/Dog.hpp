/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_V1_DOG_HPP
#define COH_TEST_V1_DOG_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/SimpleEvolvable.hpp"

#include "coherence/io/pof/EvolvableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "common/v1/Pet.hpp"

COH_OPEN_NAMESPACE3(common, test, v1)

using namespace coherence::lang;

using coherence::io::Evolvable;
using coherence::io::SimpleEvolvable;
using coherence::io::pof::EvolvableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

class Dog
    : public class_spec<Dog,
        extends<Pet> >
    {
    friend class factory<Dog>;

    // ----- constructors ---------------------------------------------------

    protected:
        Dog()
            : f_vsBreed(self()),
              f_hEvolvable(self(), SimpleEvolvable::create(1))
            {
            }

        Dog(String::View vsName, String::View vsBreed)
            : super(vsName),
              f_vsBreed(self(), vsBreed),
              f_hEvolvable(self(), SimpleEvolvable::create(1))
            {
            }

    // ----- Dog interface --------------------------------------------------

    public:
        virtual String::View getBreed() const
            {
            return f_vsBreed;
            }

        virtual void setBreed(String::View vsBreed)
            {
            initialize(f_vsBreed, vsBreed);
            }

    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            if (s_nTypeId == hIn->getUserTypeId())
                {
                initialize(f_vsBreed, hIn->readString(0));
                }
            else
                {
                super::readExternal(hIn);
                }
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            if (s_nTypeId == hOut->getUserTypeId())
                {
                hOut->writeString(0, f_vsBreed);
                }
            else
                {
                super::writeExternal(hOut);
                }
            }

    // ----- EvolvableObject interface --------------------------------------

    public:
        virtual Evolvable::View getEvolvable(int32_t nTypeId) const
            {
            if (s_nTypeId == nTypeId)
                {
                return f_hEvolvable;
                }
            return super::getEvolvable(nTypeId);
            }

        virtual Evolvable::Handle getEvolvable(int32_t nTypeId)
            {
            if (s_nTypeId == nTypeId)
                {
                return f_hEvolvable;
                }
            return super::getEvolvable(nTypeId);
            }

    // ----- Pet overrides --------------------------------------------------

    public:
        virtual bool matches(Object::View v) const
            {
            if (instanceof<Dog::View>(v))
                {
                Dog::View vDog = cast<Dog::View>(v);
                return super::matches(vDog) && f_vsBreed->equals(vDog->f_vsBreed);
                }
            return false;
            }

        // ----- Object overrides -----------------------------------------------

    public:
        virtual size32_t hashCode() const
            {
            size32_t result = super::hashCode();
            result = 31 * result + f_vsBreed->hashCode();
            return result;
            }

        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("Dog.v1{"
                   << "name='" << f_vsName << '\''
                   << ", breed='" << f_vsBreed << '\''
                   << '}');
            }

    // ----- static constants -----------------------------------------------

    public:
        static const int32_t s_nTypeId = 51718;

    // ----- data members ---------------------------------------------------

    private:
        FinalView<String> f_vsBreed;
        // TODO: private Color color;

        FinalHandle<Evolvable> f_hEvolvable;
    };

const int32_t Dog::s_nTypeId;

COH_REGISTER_TYPED_CLASS(Dog);

COH_CLOSE_NAMESPACE3

#endif // COH_TEST_V1_DOG_HPP
