/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_POF_ANNOTATION_OBJECTS_HPP
#define COH_POF_ANNOTATION_OBJECTS_HPP

#include "coherence/lang.ns"

#include "PofAnnotationObjects.hpp"

#include "coherence/io/AbstractEvolvable.hpp"

#include "coherence/io/pof/PofAnnotationSerializer.hpp"
#include "coherence/io/pof/SystemPofContext.hpp"

#include "coherence/io/pof/annotation/Portable.hpp"
#include "coherence/io/pof/annotation/PortableProperty.hpp"

#include "coherence/io/pof/reflect/Codecs.hpp"

#include "coherence/util/LinkedList.hpp"
#include "coherence/util/List.hpp"

#include <typeinfo>

using namespace coherence::lang;
using namespace std;

using coherence::io::AbstractEvolvable;

using coherence::io::pof::annotation::Portable;
using coherence::io::pof::annotation::PortableProperty;
using coherence::io::pof::reflect::Codec;
using coherence::io::pof::reflect::Codecs;

using coherence::util::Collection;
using coherence::util::LinkedList;
using coherence::util::List;

namespace annotations_test
    {

    class Poffed
        : public class_spec<Poffed,
              extends<AbstractEvolvable> >
        {
        friend class factory<Poffed>;

        protected:
            Poffed(String::View vsFirstName = String::null_string,
                   String::View vsLastName  = String::null_string, int32_t nAge = 0, bool fAdult = false)
                   : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge),
                     m_fAdult(fAdult)
                {
                }

        public:
            String::View getFirstName() const
                {
                return m_vsFirstName;
                }

            void setFirstName( String::View vsFirstName )
                {
                m_vsFirstName = vsFirstName;
                }

            String::View getLastName()  const
                {
                return m_vsLastName;
                }

            void setLastName( String::View vsLastName )
                {
                m_vsLastName = vsLastName;
                }

            int32_t      getAge() const
                {
                return m_nAge;
                }

            void setAge(int32_t nAge)
                {
                m_nAge = nAge;
                }

            bool      isAdult() const
                {
                return m_fAdult;
                }

            void setAdult(bool fAdult)
                {
                m_fAdult = fAdult;
                }

            int32_t getImplVersion() const
                {
                return 0;
                }

        private:
            MemberView<String> m_vsFirstName;
            MemberView<String> m_vsLastName;
            int32_t            m_nAge;
            bool               m_fAdult;
        };
    COH_REGISTER_CLASS(TypedClass<Poffed>::create()
            ->annotate(Portable::create())
            ->declare( COH_CONSTRUCTOR3(Poffed, String::View, String::View, BoxHandle<const Integer32>))
            ->declare( COH_PROPERTY(Poffed, FirstName, String::View)->annotate(PortableProperty::create(2)))
            ->declare( COH_PROPERTY(Poffed, LastName,  String::View)->annotate(PortableProperty::create(0, SystemClassLoader::getInstance()->loadByType(typeid(Object)))))
            ->declare( COH_PROPERTY(Poffed, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create(1)))
            ->declare( COH_METHOD0(BoxHandle<const Boolean>, Poffed::View, isAdult))
            ->declare( COH_METHOD1(void, Poffed::Handle, setAdult, BoxHandle<const Boolean>) )->annotate(PortableProperty::create(3))
            );
    COH_REGISTER_POF_ANNOTATED_CLASS(4001,Poffed);

    typedef Poffed Person;

    typedef Poffed PersonV1;

    class PersonV2
        : public class_spec<PersonV2,
              extends<AbstractEvolvable> >
        {
        friend class factory<PersonV2>;

        protected:
            PersonV2(String::View vsFirstName = String::null_string,
                   String::View vsLastName  = String::null_string, int32_t nAge = 0, bool fAdult = false,
                   Boolean::View fMale = Boolean::valueOf(false))
                   : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge),
                     m_fAdult(fAdult), m_fMale(self(), fMale)
                {
                }

        public:
            String::View getFirstName() const
                {
                return m_vsFirstName;
                }

            void setFirstName( String::View vsFirstName )
                {
                m_vsFirstName = vsFirstName;
                }

            String::View getLastName()  const
                {
                return m_vsLastName;
                }

            void setLastName( String::View vsLastName )
                {
                m_vsLastName = vsLastName;
                }

            int32_t      getAge() const
                {
                return m_nAge;
                }

            void setAge(int32_t nAge)
                {
                m_nAge = nAge;
                }

            bool      isAdult() const
                {
                return m_fAdult;
                }

            void setAdult(bool fAdult)
                {
                m_fAdult = fAdult;
                }

            Boolean::View isMale() const
                {
                return m_fMale;
                }

            void setMale(Boolean::View fMale)
                {
                m_fMale = fMale;
                }

            int32_t getImplVersion() const
                {
                return 1;
                }

        private:
            MemberView<String>   m_vsFirstName;
            MemberView<String>   m_vsLastName;
            int32_t              m_nAge;
            bool                 m_fAdult;
            MemberView<Boolean>  m_fMale;
        };
    COH_REGISTER_CLASS(TypedClass<PersonV2>::create()
            ->annotate(Portable::create())
            ->declare( COH_CONSTRUCTOR3(PersonV2, String::View, String::View, BoxHandle<const Integer32>))
            ->declare( COH_PROPERTY(PersonV2, FirstName, String::View)->annotate(PortableProperty::create(2)))
            ->declare( COH_PROPERTY(PersonV2, LastName,  String::View)->annotate(PortableProperty::create(0, SystemClassLoader::getInstance()->loadByType(typeid(Object)))))
            ->declare( COH_PROPERTY(PersonV2, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create(1)))
            ->declare( COH_METHOD0(BoxHandle<const Boolean>, PersonV2::View, isAdult)->annotate(PortableProperty::create(3)))
            ->declare( COH_METHOD1(void, PersonV2::Handle, setAdult, BoxHandle<const Boolean>) )
            ->declare( COH_METHOD0(Boolean::View, PersonV2::View, isMale)->annotate(PortableProperty::create(4)))
            ->declare( COH_METHOD1(void, PersonV2::Handle, setMale, Boolean::View) )
            );

    class PoffedImpliedIndicies : public class_spec<PoffedImpliedIndicies>
            {
            friend class factory<PoffedImpliedIndicies>;

            protected:
                PoffedImpliedIndicies(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0)
                       : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge)
                    {
                    }

            public:
                String::View getFirstName() const
                    {
                    return m_vsFirstName;
                    }

                void setFirstName( String::View vsFirstName )
                    {
                    m_vsFirstName = vsFirstName;
                    }

                String::View getLastName()  const
                    {
                    return m_vsLastName;
                    }

                void setLastName( String::View vsLastName )
                    {
                    m_vsLastName = vsLastName;
                    }

                int32_t      getAge() const
                    {
                    return m_nAge;
                    }

                void setAge(int32_t nAge)
                    {
                    m_nAge = nAge;
                    }

            private:
                MemberView<String> m_vsFirstName;
                MemberView<String> m_vsLastName;
                int32_t            m_nAge;
            };
        COH_REGISTER_CLASS(TypedClass<PoffedImpliedIndicies>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR3(PoffedImpliedIndicies, String::View, String::View, BoxHandle<const Integer32>))
                ->declare( COH_PROPERTY  (PoffedImpliedIndicies, FirstName, String::View)->annotate(PortableProperty::create(1)))
                ->declare( COH_PROPERTY  (PoffedImpliedIndicies, LastName,  String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY  (PoffedImpliedIndicies, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create(-1)))
                );

    class PoffedClashingIndicies : public class_spec<PoffedClashingIndicies>
            {
            friend class factory<PoffedClashingIndicies>;

            protected:
                PoffedClashingIndicies(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0, int32_t nAge2 = 0)
                       : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge), m_nAge2(nAge2)
                    {
                    }

            public:
                String::View getFirstName() const
                    {
                    return m_vsFirstName;
                    }

                void setFirstName( String::View vsFirstName )
                    {
                    m_vsFirstName = vsFirstName;
                    }

                String::View getLastName()  const
                    {
                    return m_vsLastName;
                    }

                void setLastName( String::View vsLastName )
                    {
                    m_vsLastName = vsLastName;
                    }

                int32_t      getAge() const
                    {
                    return m_nAge;
                    }

                void setAge(int32_t nAge)
                    {
                    m_nAge = nAge;
                    }

                int32_t      getAge2() const
                    {
                    return m_nAge2;
                    }

                void setAge2(int32_t nAge2)
                    {
                    m_nAge2 = nAge2;
                    }

            private:
                MemberView<String> m_vsFirstName;
                MemberView<String> m_vsLastName;
                int32_t            m_nAge;
                int32_t            m_nAge2;
            };
        COH_REGISTER_CLASS(TypedClass<PoffedClashingIndicies>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR3(PoffedClashingIndicies, String::View, String::View, BoxHandle<const Integer32>))
                ->declare( COH_PROPERTY(PoffedClashingIndicies, FirstName, String::View)->annotate(PortableProperty::create(1)))
                ->declare( COH_PROPERTY(PoffedClashingIndicies, LastName,  String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(PoffedClashingIndicies, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create(1)))
                ->declare( COH_PROPERTY(PoffedClashingIndicies, Age2    ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create(-1)))
                );

    class LinkedListCodec
        : public class_spec<LinkedListCodec,
               extends<Object>,
               implements<Codec> >
        {
        friend class factory<LinkedListCodec>;

        // ----- Codec members ----------------------------------------------

        public:
            /**
            * {@inheritDoc}
            */
            void encode(PofWriter::Handle hOut, int32_t nIndex, Object::View ovValue) const
                {
                hOut->writeCollection(nIndex, cast<Collection::View>(ovValue));
                }

            /**
            * {@inheritDoc}
            */
            Object::Holder decode(PofReader::Handle hIn, int32_t nIndex) const
                {
                LinkedList::Handle hLinkeList = LinkedList::create();
                return hIn->readCollection(nIndex, hLinkeList);
                }
        };
    COH_REGISTER_TYPED_CLASS(LinkedListCodec);

    class PoffedCustomCodec : public class_spec<PoffedCustomCodec>
            {
            friend class factory<PoffedCustomCodec>;

            protected:
                PoffedCustomCodec(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0, int32_t nAge2 = 0)
                       : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge), m_nAge2(nAge2)
                    {
                    }

            public:
                String::View getFirstName() const
                    {
                    return m_vsFirstName;
                    }

                void setFirstName( String::View vsFirstName )
                    {
                    m_vsFirstName = vsFirstName;
                    }

                String::View getLastName()  const
                    {
                    return m_vsLastName;
                    }

                void setLastName( String::View vsLastName )
                    {
                    m_vsLastName = vsLastName;
                    }

                int32_t getAge() const
                    {
                    return m_nAge;
                    }

                void setAge(int32_t nAge)
                    {
                    m_nAge = nAge;
                    }

                List::Handle getAliases() const
                    {
                    return m_aliases;
                    }

                void setAliases(List::Handle hAliases)
                    {
                    m_aliases = hAliases;
                    }

            private:
                MemberView<String> m_vsFirstName;
                MemberView<String> m_vsLastName;
                int32_t            m_nAge;
                int32_t            m_nAge2;
                List::Handle       m_aliases;
            };
        COH_REGISTER_CLASS(TypedClass<PoffedCustomCodec>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR3(PoffedCustomCodec, String::View, String::View, BoxHandle<const Integer32>))
                ->declare( COH_PROPERTY(PoffedCustomCodec, FirstName, String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(PoffedCustomCodec, LastName,  String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(PoffedCustomCodec, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(PoffedCustomCodec, Aliases ,  List::Handle)->annotate(PortableProperty::create(-1,
                        SystemClassLoader::getInstance()->loadByType(typeid(LinkedListCodec)) )))
                );

        class Child : public class_spec<Child>
            {
            friend class factory<Child>;

            protected:
                Child(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0)
                       : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge)
                    {
                    }

            public:

                String::View getFirstName() const
                    {
                    return m_vsFirstName;
                    }

                void setFirstName( String::View vsFirstName )
                    {
                    m_vsFirstName = vsFirstName;
                    }

                String::View getLastName()  const
                    {
                    return m_vsLastName;
                    }

                void setLastName( String::View vsLastName )
                    {
                    m_vsLastName = vsLastName;
                    }

                int32_t      getAge() const
                    {
                    return m_nAge;
                    }

                void setAge(int32_t nAge)
                    {
                    m_nAge = nAge;
                    }

            private:
                MemberView<String> m_vsFirstName;
                MemberView<String> m_vsLastName;
                int32_t            m_nAge;
            };
        COH_REGISTER_CLASS(TypedClass<Child>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR3(Child, String::View, String::View, BoxHandle<const Integer32>))
                ->declare( COH_PROPERTY(Child, FirstName, String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(Child, LastName,  String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(Child, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create()))
                );

        class Father : public class_spec<Father>
            {
            friend class factory<Father>;

            protected:
                Father(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0,
                       Child::View vChild = NULL)
                       : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge),
                         m_vChild(self(), vChild)
                    {
                    }

            public:
                String::View getFirstName() const
                    {
                    return m_vsFirstName;
                    }

                void setFirstName(String::View vsFirstName)
                    {
                    m_vsFirstName = vsFirstName;
                    }

                String::View getLastName() const
                    {
                    return m_vsLastName;
                    }

                void setLastName(String::View vsLastName)
                    {
                    m_vsLastName = vsLastName;
                    }

                int32_t getAge() const
                    {
                    return m_nAge;
                    }

                void setAge(int32_t nAge)
                    {
                    m_nAge = nAge;
                    }

                Child::View getChild() const
                    {
                    return m_vChild;
                    }

                void setChild(Child::View vChild)
                    {
                    m_vChild = vChild;
                    }

            private:
                MemberView<String> m_vsFirstName;
                MemberView<String> m_vsLastName;
                int32_t            m_nAge;
                MemberView<Child>  m_vChild;
            };
        COH_REGISTER_CLASS(TypedClass<Father>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR3(Father, String::View, String::View, BoxHandle<const Integer32>))
                ->declare( COH_PROPERTY(Father, FirstName, String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(Father, LastName,  String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(Father, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(Father, Child   ,  Child::View)->annotate(PortableProperty::create()))
                );

        class GrandFather : public class_spec<GrandFather>
            {
            friend class factory<GrandFather>;

            protected:
                GrandFather(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0,
                       Father::View vFather = NULL)
                       : m_vsFirstName(self(), vsFirstName), m_vsLastName(self(), vsLastName), m_nAge(nAge),
                         m_vFather(self(), vFather)
                    {
                    }

            public:
                String::View getFirstName() const
                    {
                    return m_vsFirstName;
                    }

                void setFirstName(String::View vsFirstName)
                    {
                    m_vsFirstName = vsFirstName;
                    }

                String::View getLastName() const
                    {
                    return m_vsLastName;
                    }

                void setLastName(String::View vsLastName)
                    {
                    m_vsLastName = vsLastName;
                    }

                int32_t getAge() const
                    {
                    return m_nAge;
                    }

                void setAge(int32_t nAge)
                    {
                    m_nAge = nAge;
                    }

                Father::View getFather() const
                    {
                    return m_vFather;
                    }

                void setFather(Father::View vFather)
                    {
                    m_vFather = vFather;
                    }

            private:
                MemberView<String> m_vsFirstName;
                MemberView<String> m_vsLastName;
                int32_t            m_nAge;
                MemberView<Father> m_vFather;
            };
        COH_REGISTER_CLASS(TypedClass<GrandFather>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR3(GrandFather, String::View, String::View, BoxHandle<const Integer32>))
                ->declare( COH_PROPERTY(GrandFather, FirstName, String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(GrandFather, LastName,  String::View)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(GrandFather, Age     ,  BoxHandle<const Integer32>)->annotate(PortableProperty::create()))
                ->declare( COH_PROPERTY(GrandFather, Father  ,  Father::View)->annotate(PortableProperty::create()))
                );

        class BewilderedPerson : public class_spec<BewilderedPerson,
                                    extends<Person> >
            {
            friend class factory<BewilderedPerson>;

            protected:
                BewilderedPerson(String::View vsFirstName = String::null_string,
                       String::View vsLastName  = String::null_string, int32_t nAge = 0,
                       String::View vsState = String::null_string)
                       : super(vsFirstName, vsLastName, nAge), m_vsState(self(),vsState)
                    {
                    }

            public:
                String::View getState() const
                    {
                    return m_vsState;
                    }

                void setState(String::View vsState)
                    {
                    m_vsState = vsState;
                    }

            private:
                MemberView<String> m_vsState;
            };
        COH_REGISTER_CLASS(TypedClass<BewilderedPerson>::create()
                ->annotate(Portable::create())
                ->declare( COH_CONSTRUCTOR4(BewilderedPerson, String::View, String::View, BoxHandle<const Integer32>, String::View))
                ->declare( COH_PROPERTY(BewilderedPerson, State, String::View)->annotate(PortableProperty::create()))
                );
    }

#endif // COH_POF_ANNOTATION_OBJECTS_HPP
