/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_SERIALIZER_TEST_DATA_HPP
#define COH_SERIALIZER_TEST_DATA_HPP

#include "coherence/lang.ns"

COH_OPEN_NAMESPACE2(common, test)

using namespace coherence::lang;


/**
* Test data class definitions for the SerializerTest.
*
* @author lh  2011.07.15
*/
class SerializerTestData
    : public class_spec<SerializerTestData,
        extends<Object> >
    {
    friend class factory<SerializerTestData>;

    public:
        class Balance
            : public class_spec<Balance,
                extends<Object> >
            {
            friend class factory<Balance>;

            protected:
                Balance()
                    : m_dflBalance(0.0f)
                    {
                    }

            public:
                float32_t getBalance() const
                    {
                    return m_dflBalance;
                    }

                void setBalance(float32_t dfl)
                    {
                    m_dflBalance = dfl;
                    }

            private:
                float32_t m_dflBalance;
            };

    public:
        class Product
            : public class_spec<Product,
                extends<Object> >
            {
            friend class factory<Product>;

            protected:
                Product()
                    : m_vBalance(self(), NULL)
                    {
                    }

                Product(Balance::View vBal)
                    : m_vBalance(self(), vBal)
                    {
                    }

            public:
                Balance::View getBalance() const
                    {
                    return m_vBalance;
                    }

                void setBalance(Balance::View vBal)
                    {
                    m_vBalance = vBal;
                    }

            private:
                MemberView<Balance> m_vBalance;
            };

    public:
        class Customer
            : public class_spec<Customer,
                extends<Object> >
            {
            friend class factory<Customer>;

            protected:
                Customer()
                    : m_vsName(self(), String::null_string), m_vBalance(self(), NULL),
                      m_vProduct(self(), NULL)
                    {
                    }

                Customer(String::View vsName)
                    : m_vsName(self(), vsName), m_vBalance(self(), NULL),
                      m_vProduct(self(), NULL)
                    {
                    }

                Customer(String::View vsName, Balance::View vBal, Product::View vP)
                    : m_vsName(self(), vsName), m_vBalance(self(), vBal),
                      m_vProduct(self(), vP)
                    {
                    }

            public:
                String::View getName() const
                    {
                    return m_vsName;
                    }

                void setName(String::View vsName)
                    {
            	    m_vsName = vsName;
                    }

                Balance::View getBalance() const
                    {
                    return m_vBalance;
                    }

                void setBalance(Balance::View vBal)
                    {
                    m_vBalance = vBal;
                    }

                Product::View getProduct() const
                    {
                    return m_vProduct;
                    }

                void setProduct(Product::View vProduct)
                    {
                    m_vProduct = vProduct;
                    }

            private:
                MemberView<String>  m_vsName;
                MemberView<Balance> m_vBalance;
                MemberView<Product> m_vProduct;
            };
    };

COH_CLOSE_NAMESPACE2

#endif // COH_SERIALIZER_TEST_DATA_HPP
