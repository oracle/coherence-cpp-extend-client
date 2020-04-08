/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_TEST_V2_PET_HPP
#define COH_TEST_V2_PET_HPP

#include "coherence/lang.ns"

#include "coherence/io/Evolvable.hpp"
#include "coherence/io/SimpleEvolvable.hpp"

#include "coherence/io/pof/EvolvableObject.hpp"
#include "coherence/io/pof/PofReader.hpp"
#include "coherence/io/pof/PofWriter.hpp"

#include "common/v2/Animal.hpp"

COH_OPEN_NAMESPACE3(common, test, v2)

using namespace coherence::lang;

using coherence::io::Evolvable;
using coherence::io::SimpleEvolvable;
using coherence::io::pof::EvolvableObject;
using coherence::io::pof::PofReader;
using coherence::io::pof::PofWriter;

class Pet
    : public class_spec<Pet,
        extends<Animal> >
    {
    friend class factory<Pet>;

    // ----- constructors ---------------------------------------------------

    protected:
        Pet()
            : f_vsName(self()),
              m_cAge(),
              f_hEvolvable(self(), SimpleEvolvable::create(2))
            {
            }

        Pet(String::View vSpecies, String::View vsName, int age)
            : super(vSpecies),
              f_vsName(self(), vsName),
              m_cAge(age),
              f_hEvolvable(self(), SimpleEvolvable::create(2))
            {
            }

    // ----- PortableObject interface ---------------------------------------

    public:
        virtual void readExternal(PofReader::Handle hIn)
            {
            if (hIn->getUserTypeId() == s_nTypeId)
                {
                initialize(f_vsName, hIn->readString(0));
                if (hIn->getVersionId() > 1)
                    {
                    m_cAge = hIn->readInt32(1);
                    }
                }
            else
                {
                super::readExternal(hIn);
                }
            }

        virtual void writeExternal(PofWriter::Handle hOut) const
            {
            if (hOut->getUserTypeId() == s_nTypeId)
                {
                hOut->writeString(0, f_vsName);
                hOut->writeInt32(1, m_cAge);
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
            if (nTypeId == s_nTypeId)
                {
                return f_hEvolvable;
                }
            return super::getEvolvable(nTypeId);
            }

        virtual Evolvable::Handle getEvolvable(int32_t nTypeId)
            {
            if (nTypeId == s_nTypeId)
                {
                return f_hEvolvable;
                }
            return super::getEvolvable(nTypeId);
            }

    // ----- Animal overrides -----------------------------------------------

    public:
        virtual bool matches(Object::View v) const
            {
            if (instanceof<Pet::View>(v))
                {
                Pet::View vPet = cast<Pet::View>(v);
                return super::matches(vPet) && m_cAge == vPet->m_cAge && f_vsName->equals(
                        vPet->f_vsName);
                }
            return false;
            }

    // ----- Object overrides -----------------------------------------------

    public:
        virtual size32_t hashCode() const
            {
            size32_t result = super::hashCode();
            result = 31 * result + f_vsName->hashCode();
            result = 31 * result + m_cAge;
            return result;
            }

        virtual TypedHandle<const String> toString() const
            {
            return COH_TO_STRING("Pet.v2{" <<
                   "name='" << f_vsName << '\''
                   << ", age=" << m_cAge
                   << ", species=" << f_vsSpecies
                   << '}');
            }

    // ----- static constants -----------------------------------------------

    public:
        static const int32_t s_nTypeId = 51717;

    // ----- data members ---------------------------------------------------

    protected:
        FinalView<String> f_vsName;
        int m_cAge;

    private:
        FinalHandle<Evolvable> f_hEvolvable;
    };

const int32_t Pet::s_nTypeId;

COH_REGISTER_TYPED_CLASS(Pet);
// Intentionally use the "REGISTER_ONLY" version here to verify it works.
// Animal and Dog use the "COH_REGISTER_PORTABLE..." version
COH_REGISTER_ONLY_PORTABLE_TYPE_CLASS(Pet::s_nTypeId, Pet);

COH_CLOSE_NAMESPACE3

#endif // COH_TEST_V2_PET_HPP
