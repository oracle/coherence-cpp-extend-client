/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXPECTATION_HPP
#define COH_EXPECTATION_HPP

#include "coherence/lang.ns"
#include "coherence/util/ArrayList.hpp"

COH_OPEN_NAMESPACE(mock)

using coherence::util::ArrayList;

/**
* TODO : Doc this class
*
* @author tb  2008.02.29
*/
class Expectation
    : public class_spec<Expectation>
    {
    friend class factory<Expectation>;

    // ----- handle definitions ---------------------------------------------

    public:
        /**
        * Argument matcher function
        */
        typedef bool(*ptrArgMatcher)(ArrayList::View vlistExpected,
                                     ArrayList::View vlistActual);


    // ----- constructors ---------------------------------------------------

    protected:
        Expectation(String::View signature)
                : m_vSignature(self(), signature),
                  m_ohReturnObject(self()),
                  m_hArguments(self(), ArrayList::create()),
                  m_hException(self()),
                  m_fIgnoreArguments(false),
                  m_fArgsSet(false),
                  m_ptrArgMatcher(NULL)
            {
            }


    // ----- Expectation interface ------------------------------------------

    public:

        virtual int16_t getInt16Return()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Integer16::Handle>(m_ohReturnObject)->getValue();
            }

        virtual void setInt16Return(int16_t iVal)
            {
            m_ohReturnObject = Integer16::create(iVal);
            }

        virtual int32_t getInt32Return()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Integer32::Handle>(m_ohReturnObject)->getValue();
            }

        virtual void setInt32Return(int32_t iVal)
            {
            m_ohReturnObject = Integer32::create(iVal);
            }

        virtual int64_t getInt64Return()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Integer64::Handle>(m_ohReturnObject)->getValue();
            }

        virtual void setInt64Return(int64_t iVal)
            {
            m_ohReturnObject = Integer64::create(iVal);
            }


        virtual float32_t getFloat32Return()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Float32::Handle>(m_ohReturnObject)->getValue();
            }

        virtual void setFloat32Return(float32_t fVal)
            {
            m_ohReturnObject = Float32::create(fVal);
            }

        virtual float64_t getFloat64Return()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Float64::Handle>(m_ohReturnObject)->getValue();
            }

        virtual void setFloat64Return(float64_t fVal)
            {
            m_ohReturnObject = Float64::create(fVal);
            }

        virtual wchar16_t getChar16Return()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Character16::Handle>(m_ohReturnObject)->getValue();
            }

        virtual void setChar16Return(wchar16_t cVal)
            {
            m_ohReturnObject = Character16::create(cVal);
            }

        //**** TODO : Deprecate these ***/
        int32_t getIntegerReturn()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Integer32::Handle>(m_ohReturnObject)->getValue();
            }

        void setIntegerReturn(int32_t iVal)
            {
            m_ohReturnObject = Integer32::create(iVal);
            }

        int64_t getLongIntegerReturn()
            {
            return is_null(m_ohReturnObject) ?
                    0 : cast<Integer64::Handle>(m_ohReturnObject)->getValue();
            }

        void setLongIntegerReturn(int64_t iVal)
            {
            m_ohReturnObject = Integer64::create(iVal);
            }
        //**** TODO : Deprecate above ***/

        bool getBoolReturn()
            {
            return is_null(m_ohReturnObject) ?
                    false : cast<Boolean::Handle>(m_ohReturnObject)->getValue();
            }

        void setBoolReturn(bool bVal)
            {
            m_ohReturnObject = Boolean::create(bVal);
            }

        Object::Holder getObjectReturn()
            {
            return m_ohReturnObject;
            }

        void setObjectReturn(Object::Holder ohVal)
            {
            m_ohReturnObject = ohVal;
            }

        void setException(Exception::Handle hException)
            {
            m_hException = hException;
            }

        Exception::Handle getException()
            {
            return m_hException;
            }

        void setMatcher(ptrArgMatcher pMatcher)
            {
            m_ptrArgMatcher = pMatcher;
            }

        Expectation::Handle withInt16Argument(int16_t iArg)
            {
            checkArgsSet();
            return withObjectArgument(Integer16::create(iArg));
            }

        Expectation::Handle withInt32Argument(int32_t iArg)
            {
            checkArgsSet();
            return withObjectArgument(Integer32::create(iArg));
            }

        Expectation::Handle withInt64Argument(int64_t iArg)
            {
            checkArgsSet();
            return withObjectArgument(Integer64::create(iArg));
            }

        Expectation::Handle withFloat32Argument(float32_t fArg)
            {
            checkArgsSet();
            return withObjectArgument(Float32::create(fArg));
            }

        Expectation::Handle withFloat64Argument(float64_t fArg)
            {
            checkArgsSet();
            return withObjectArgument(Float64::create(fArg));
            }

        Expectation::Handle withChar16Argument(wchar16_t cArg)
            {
            checkArgsSet();
            return withObjectArgument(Character16::create(cArg));
            }

        //**** TODO : Deprecate these ***/
        Expectation::Handle withIntegerArgument(int32_t iArg)
            {
            checkArgsSet();
            return withObjectArgument(Integer32::create(iArg));
            }

        Expectation::Handle withLongIntegerArgument(int64_t iArg)
            {
            checkArgsSet();
            return withObjectArgument(Integer64::create(iArg));
            }
        //**** TODO : Deprecate above ***/

        Expectation::Handle withBoolArgument(bool bArg)
            {
            checkArgsSet();
            return withObjectArgument(Boolean::create(bArg));
            }

        Expectation::Handle withObjectArgument(Object::Holder ohArg)
            {
            checkArgsSet();
            m_hArguments->add(ohArg);
            return this;
            }

        void ignoreArguments()
            {
            m_fIgnoreArguments = true;
            }

        /**
         * Called after all the arguments have been set.  The next call
         * to withXXXXArgument will reset the argument list.  This allows
         * for the argument list to be changed after the expectation is set
         * by the mock.
         */
        void argsSet()
            {
            m_fArgsSet = true;
            }

        String::View getSignature() const
            {
            return m_vSignature;
            }

        TypedHandle<const String> toString() const
            {
            std::stringstream ss;
            ss << m_vSignature << " : (" << m_hArguments->size() << "): ";
            for (size32_t i = 0; i < m_hArguments->size(); ++i)
                {
                ss << m_hArguments->get(i) << " - ";
                }
            return String::create(ss.str().c_str());
            }

        bool equals(Object::View v) const
            {
            TypedHandle<const Expectation> vThat = cast<Expectation::View>(v);

            if (NULL == vThat)
                {
                return false;
                }
            else if (this == vThat)
                {
                return true;
                }
            else
                {
                if (!Object::equals(m_vSignature, vThat->m_vSignature))
                    return false;

                if (vThat->m_ptrArgMatcher != NULL)
                    {
                    return vThat->m_ptrArgMatcher (vThat->m_hArguments, m_hArguments);
                    }
                else
                    {
                    size32_t cThisSize = m_hArguments->size();
                    size32_t cThatSize = vThat->m_hArguments->size();

                    if (m_fIgnoreArguments || vThat->m_fIgnoreArguments) return true;

                    if (cThisSize!=cThatSize)
                        {
                        return false;
                        }

                    for (size32_t i = 0; i < cThisSize; ++i)
                        {
                        Object::Holder thisArg = m_hArguments->get(i);
                        Object::Holder thatArg = vThat->m_hArguments->get(i);

                        if (!(thisArg == thatArg || Object::equals(thisArg, thatArg)))
                            return false;
                        }
                    return true;
                    }
                }
            }

    private:
        void checkArgsSet()
            {
            if (m_fArgsSet)
                {
                m_hArguments->clear();
                m_fArgsSet = false;
                }
            }

    // ----- data members ---------------------------------------------------
    protected:

        MemberView<String>     m_vSignature;
        MemberHolder<Object>     m_ohReturnObject;
        MemberHandle<ArrayList> m_hArguments;
        MemberHandle<Exception>  m_hException;
        bool               m_fIgnoreArguments;
        bool               m_fArgsSet;
        // arg matcher function pointer
        ptrArgMatcher m_ptrArgMatcher;
    };

COH_CLOSE_NAMESPACE

#endif // COH_EXPECTATION_HPP
