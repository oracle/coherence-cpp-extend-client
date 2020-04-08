/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_BASEMOCK_HPP
#define COH_BASEMOCK_HPP

#include "coherence/lang.ns"
#include "mock/Expectation.hpp"
#include "coherence/util/ArrayList.hpp"
#include <iostream>

COH_OPEN_NAMESPACE(mock)

using coherence::util::ArrayList;

/**
* TODO : Doc this class
*
* @author tb  2008.02.29
*/
class BaseMock
    {

    // ----- constructors ---------------------------------------------------

    protected:
        BaseMock()
                : m_hExpectations(System::common(), ArrayList::create(), /*fMutable*/ true),
                  m_hLastExpectation(System::common(), NULL, /*fMutable*/ true),
                  m_fReplay(false),
                  m_fStrict(false),
                  m_fDebug(false),
                  m_hUnexpectedExpectation(System::common(), NULL, /*fMutable*/ true)
            {
            }

        virtual ~BaseMock()
            {}

    // ----- BaseMock interface ---------------------------------------------
    public:

        Expectation::Handle invocation(String::View signature) const
            {
            return Expectation::create(signature);
            }

        Expectation::Handle setExpectation(Expectation::Handle hExp) const
            {
            if (m_fReplay)
                {
                if (m_fDebug)
                    {
                    std::cout << "\nReplay :" << hExp->getSignature() << "\n";
                    std::cout << "\nm_hExpectations->size()=" << m_hExpectations->size() << "\n";
                    std::cout << "\nm_fStrict=" << m_fStrict << "\n";
                    }
                if (m_hExpectations->size() > 0)
                    {
                    size32_t cSize = m_hExpectations->size();
                    for (size32_t i = 0; i == 0 || (!m_fStrict && i < cSize); ++i)
                        {
                        if (m_fDebug)
                            {
                            std::cout << "\nCalling checkExpectation : " << i << "\n";
                            }

                        Expectation::Handle hSetExp = checkExpectation(hExp, i);
                        if (!is_null(hSetExp))
                            {
                            return hSetExp;
                            }
                        }
                    }
                if (m_fStrict)
                    {
                    m_hUnexpectedExpectation = hExp;
                    if (m_fDebug)
                        {
                        std::cout << "Unexpected invocation:" << hExp->getSignature() << "\n";
                        }
                    COH_THROW_STREAM(RuntimeException, "unexpected invocation:" << hExp->getSignature());
                    }
                }
            else
                {
                m_hExpectations->add(hExp);
                hExp->argsSet();
                m_hLastExpectation = hExp;
                }
            return hExp;
            }

        Expectation::Handle checkExpectation(Expectation::Handle hExp,
                size32_t cExpIndex) const
            {
            Expectation::Handle hSetExp =
                    cast<Expectation::Handle>(m_hExpectations->get(cExpIndex));


            if (m_fDebug)
                    {
                    std::cout << "\nComparing ... " << cExpIndex;
                    std::cout << "\nhExp    :" << hExp->toString() << "\n";
                    std::cout << "\nhSetExp :" << hSetExp->toString() << "\n";
                    }


            if (hExp->equals(hSetExp))
                {
                if (m_fDebug)
                        {
                        std::cout << "... equals!\n";
                        }

                m_hExpectations->remove(cExpIndex);

                Exception::Handle hException = hSetExp->getException();
                if (!is_null(hException))
                    {
                    COH_THROW(hException);
                    }
                return hSetExp;
                }
            return NULL;
            }

        void replay() const
            {
            m_fReplay = true;
            }

        void verify() const
            {
            if (m_hExpectations->size() > 0)
                {
                std::cerr << "\nVerify failure.  Expected :\n";
                size32_t cSize = m_hExpectations->size();
                for (size32_t i = 0; i < cSize; ++i)
                    {
                    Expectation::Handle hExp =
                            cast<Expectation::Handle>(m_hExpectations->get(i));
                    std::cerr << "    " << i + 1 << ". " << hExp->getSignature() << std::endl;
                    }
                COH_THROW(RuntimeException::create());
                }
            else if (m_hUnexpectedExpectation != NULL)
                {
                std::cerr << "Unexpected function invocation: " <<
                        m_hUnexpectedExpectation->getSignature() << std::endl;
                COH_THROW(RuntimeException::create(
                        COH_TO_STRING("Unexpected function invocation: " <<
                                m_hUnexpectedExpectation->getSignature())));
                }
            }

        bool verifyAndReturnResult() const
            {
            return m_hExpectations->size() == 0 && NULL == m_hUnexpectedExpectation;
            }

        Expectation::Handle lastExpectation() const
            {
            return m_hLastExpectation;
            }

        void setStrict(bool bStrict)
            {
            m_fStrict=bStrict;
            }

        void setDebug(bool bDebug)
            {
            m_fDebug=bDebug;
            }


    // ----- last Expectation methods ---------------------------------------
    public:

        virtual void setMatcher(Expectation::ptrArgMatcher pArgMatcher)
            {
            return m_hLastExpectation->setMatcher(pArgMatcher);
            }

        virtual int16_t getInt16Return()
            {
            return m_hLastExpectation->getInt16Return();
            }

        virtual void setInt16Return(int16_t iVal)
            {
            m_hLastExpectation->setInt16Return(iVal);
            }

        virtual int32_t getInt32Return()
            {
            return m_hLastExpectation->getInt32Return();
            }

        virtual void setInt32Return(int32_t iVal)
            {
            m_hLastExpectation->setInt32Return(iVal);
            }

        virtual int64_t getInt64Return()
            {
            return m_hLastExpectation->getInt64Return();
            }

        virtual void setInt64Return(int64_t iVal)
            {
            m_hLastExpectation->setInt64Return(iVal);
            }


        virtual float32_t getFloat32Return()
            {
            return m_hLastExpectation->getFloat32Return();
            }

        virtual void setFloat32Return(float32_t fVal)
            {
            m_hLastExpectation->setFloat32Return(fVal);
            }

        virtual float64_t getFloat64Return()
            {
            return m_hLastExpectation->getFloat64Return();
            }

        virtual void setFloat64Return(float64_t fVal)
            {
            m_hLastExpectation->setFloat64Return(fVal);
            }

        virtual wchar16_t getChar16Return()
            {
            return m_hLastExpectation->getChar16Return();
            }

        virtual void setFloat64Return(wchar16_t cVal)
            {
            m_hLastExpectation->setFloat64Return(cVal);
            }

        //**** TODO : Deprecate these ***/
        int32_t getIntegerReturn()
            {
            return m_hLastExpectation->getIntegerReturn();
            }

        void setIntegerReturn(int32_t iVal)
            {
            m_hLastExpectation->setIntegerReturn(iVal);
            }

        int64_t getLongIntegerReturn()
            {
            return m_hLastExpectation->getLongIntegerReturn();
            }

        void setLongIntegerReturn(int64_t iVal)
            {
            m_hLastExpectation->setLongIntegerReturn(iVal);
            }
        //**** TODO : Deprecate above ***/

        bool getBoolReturn()
            {
            return m_hLastExpectation->getBoolReturn();
            }

        void setBoolReturn(bool bVal)
            {
            m_hLastExpectation->setBoolReturn(bVal);
            }

        Object::Holder getObjectReturn()
            {
            return m_hLastExpectation->getObjectReturn();
            }

        void setObjectReturn(Object::Holder ohVal)
            {
            m_hLastExpectation->setObjectReturn(ohVal);
            }

        Exception::Handle getException()
            {
            return m_hLastExpectation->getException();
            }

        void setException(Exception::Handle hException)
            {
            m_hLastExpectation->setException(hException);
            }

        Expectation::Handle withInt16Argument(int16_t iArg)
            {
            return m_hLastExpectation->withInt16Argument(iArg);
            }

        Expectation::Handle withInt32Argument(int32_t iArg)
            {
            return m_hLastExpectation->withInt32Argument(iArg);
            }

        Expectation::Handle withInt64Argument(int64_t iArg)
            {
            return m_hLastExpectation->withInt64Argument(iArg);
            }

        Expectation::Handle withFloat32Argument(float32_t fArg)
            {
            return m_hLastExpectation->withFloat32Argument(fArg);
            }

        Expectation::Handle withFloat64Argument(float64_t fArg)
            {
            return m_hLastExpectation->withFloat64Argument(fArg);
            }

        Expectation::Handle withChar16Argument(wchar16_t cArg)
            {
            return m_hLastExpectation->withChar16Argument(cArg);
            }

        //**** TODO : Deprecate these ***/
        Expectation::Handle withIntegerArgument(int32_t iArg)
            {
            return m_hLastExpectation->withIntegerArgument(iArg);
            }

        Expectation::Handle withLongIntegerArgument(int64_t iArg)
            {
            return m_hLastExpectation->withLongIntegerArgument(iArg);
            }
        //**** TODO : Deprecate above ***/

        Expectation::Handle withBoolArgument(bool bArg)
            {
            return m_hLastExpectation->withBoolArgument(bArg);
            }

        Expectation::Handle withObjectArgument(Object::Holder ohArg)
            {
            return m_hLastExpectation->withObjectArgument(ohArg);
            }

    // ----- data members ---------------------------------------------------
    protected:

        mutable MemberHandle<ArrayList> m_hExpectations;
        mutable MemberHandle<Expectation> m_hLastExpectation;
        mutable bool m_fReplay;
        mutable bool m_fStrict;
        mutable bool m_fDebug;
        mutable MemberView<Expectation> m_hUnexpectedExpectation;
    };

COH_CLOSE_NAMESPACE

#endif // COH_BASEMOCK_HPP
