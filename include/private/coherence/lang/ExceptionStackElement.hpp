/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXCEPTION_STACK_ELEMENT_HPP
#define COH_EXCEPTION_STACK_ELEMENT_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/StackTraceElement.hpp"
#include "coherence/lang/String.hpp"



COH_OPEN_NAMESPACE2(coherence,lang)


/**
* ExceptionStackElement provides basic file, line, function trace information
* representing the point at which an exception was thrown.
*
* @author mf  2008.04.01
*/
class COH_EXPORT ExceptionStackElement
    : public class_spec<ExceptionStackElement,
        extends<Object>,
        implements<StackTraceElement> >
    {
    friend class factory<ExceptionStackElement>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new ExceptionStackElement.
        *
        * @param vsFile      the file associated with the stack element
        * @param nLine       the line number associated with the stack element
        * @param vsFunction  the function associated with the stack element
        *
        * @return a new ExceptionStackElement
        */
        ExceptionStackElement(String::View vsFile, int32_t nLine,
                String::View vsFunction);


    // ----- ExceptionStackElement interface --------------------------------

    public:
        /**
        * Return the name of the file where the exception was thrown
        *
        * @return the exception's origin file name
        */
        virtual String::View getFile() const;

        /**
        * Return line number where the Exception was thrown.
        *
        * @return the line number where the Exception was thrown
        */
        virtual int32_t getLine() const;

        /**
        * Return the name and signature of the function from which the
        * exception was thrown.
        *
        * @return the name and signature of the function from which the
        *         exception was thrown
        */
        virtual String::View getFunction() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The file from which the exception was thrown.
        */
        FinalView<String> f_vsFile;

        /**
        * The line at which the exception was thrown.
        */
        const int32_t m_nLine;

        /**
        * The function at which the exception was thrown.
        */
        FinalView<String> f_vsFunction;
    };

COH_CLOSE_NAMESPACE2

#endif // COH_EXCEPTION_STACK_ELEMENT_HPP
