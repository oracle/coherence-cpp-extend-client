/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_EXCEPTION_HPP
#define COH_EXCEPTION_HPP

#include "coherence/lang/compatibility.hpp"

#include "coherence/lang/throwable_spec.hpp"
#include "coherence/lang/FinalView.hpp"
#include "coherence/lang/ObjectArray.hpp"
#include "coherence/lang/String.hpp"
#include "coherence/lang/TypedHandle.hpp"
#include "coherence/lang/TypedHolder.hpp"

#include <stdexcept>

COH_OPEN_NAMESPACE2(coherence,lang)


/**
* Base class for all exceptions used in Coherence.
*
* Exceptions are not thrown directly, but rather via the COH_THROW macro. The
* macro will record the stack trace and throw the managed Exception such that
* it may be caught either via its View type, or as a std::exception derivative.
* Unlike standard C++ exceptions, managed exceptions may be caught by value,
* i.e. not using a const& and still be safely re-thrown
* (via the COH_THROW macro) without risking object slicing. This allows caught
* exceptions to be stored as data member or local variables outside of a
* try/catch block and to be safely re-thrown at a later time.
*
* New exception classes are declared using the throwable_spec<> helper template.
*
* @code
* try
*   {
*   ...
*   COH_THROW (IOException::create("some error"));
*   ...
*   }
* catch (IOException::View vIoe)
*   {
*   std::cerr << vIoe << std::endl;
*   ...
*   }
* catch (Exception::View vEx)
*   {
*   std::cerr << vEx << std::endl;
*   ...
*   COH_THROW (vEx); // re-throw managed exception
*   }
* catch (const std::exception& e)
*   {
*   std::cerr << e.what() << std::endl;
*   throw; // re-throw standard exception
*   }
* @endcode
*
* @see throwable
*
* @author mf 2007.05.05
*/
class COH_EXPORT Exception
    : public throwable_spec<Exception,
        extends<Object, std::exception>,
        implements<>,
        Object::View>
    {
    friend class factory<Exception>;

    // ----- constructors ---------------------------------------------------

    protected:
        /**
        * Create a new Exception object
        *
        * @param vsMsg    the message of the exception
        * @param veCause  the underlying cause of the exception;
        *                 can be <tt>NULL</tt>
        *
        * @return a Handle to the created Exception
        */
        Exception(String::View vsMsg = String::null_string,
                Exception::View veCause = NULL);

        /**
        * Copy constructor
        */
        Exception(const Exception&);


    // ----- Exception interface --------------------------------------------

    public:
        /**
        * Return the name of the exception.
        */
        virtual String::View getName() const;

        /**
        * Returns a human-readable description of the Exception.
        *
        * Note: The String returned is held for the lifetime of this exception
        * to guarantee that the result does not go out of scope. This
        * method is used by Exception Handles to support the
        * std::exception::what() method.
        *
        * @return the Exception's description
        */
        virtual String::View getDescription() const;

        /**
        * Set the message associated with this exception.
        *
        * @param vsMsg  the message to set for this exception
        */
        virtual void setMessage(String::View vsMsg);

        /**
        * Return the message associated with this exception.
        *
        * @return the message associated with this exception
        */
        virtual String::View getMessage() const;

        /**
        * Return the underlying cause associated with this exception.
        * The underlying cause is the exception that caused this exception
        * to be thrown.
        *
        * @return the underlying cause associated with this exception;
        *         might be <tt>NULL</tt>
        */
        virtual Exception::View getCause() const;

        /**
        * Set the name of the thread on which this exception was thrown.
        *
        * @param vsThreadName  the thread name
        */
        virtual void setThreadName(String::View vsThreadName);

        /**
        * Return the name of the thread on which the exception was thrown.
        *
        * @return the name of the thread on which the exception was thrown.
        */
        virtual String::View getThreadName() const;

        /**
        * Set point at which the exception occurred.
        *
        * @param vaFrames    an array of StackTraceElements
        */
        virtual void setStackTrace(ObjectArray::View vaFrames);

        /**
        * Return the stack trace for the exception.
        *
        * @return an array of StackTraceElements.
        */
        virtual ObjectArray::View getStackTrace() const;

        /**
        * Fills the execution stack trace based on the current threads stack
        * and the supplied information about the current stack frame.
        *
        * @param vsFile      the file portion of the first stack frame
        * @param nLine       the lone portion of the first stack frame
        * @param vsFunction  the function portion of the first stack frame
        */
        virtual Exception::Handle fillInStackTrace(
                String::View vsFile = String::null_string, int32_t nLine = 0,
                String::View vsFunction = String::null_string);

        /**
        * Format the stack trace for printing
        *
        * @return the stack trace
        */
        virtual String::View formatStackTrace() const;

        /**
        * (Re)throw the exception.
        *
        * The resulting thrown exception may be caught either by it's View
        * type, or its alias type.
        */
        virtual void raise() const;


    // ----- Object interface -----------------------------------------------

    public:
        /**
        * {@inheritDoc}
        */
        virtual TypedHandle<const String> toString() const;


    // ----- data members ---------------------------------------------------

    protected:
        /**
        * The message associated with this exception.
        */
        FinalView<String> f_vsMessage;

        /**
        * The stack at the point the exception was thrown
        */
        FinalView<ObjectArray> f_vaStackFrames;

        /**
        * The name of the thread on which the Exception was thrown;
        */
        FinalView<String> f_vsThreadName;

        /**
        * The cause of the exception
        */
        FinalView<Exception> f_veCause;

        /**
        * The detailed human readable description of this exception.
        */
        mutable FinalView<String> f_vsDescription;
    };


// ----- helper macros ------------------------------------------------------

/**
* @hideinitializer
*
* [re]throw a managed exception.  If the exception is referenced via a Handle
* it's stack trace will be set to the current stack location, otherwise the
* stack related information will unchanged.
*
* @param E  the exception to throw
*
* Usage example:
* @code
* COH_THROW(IOException::create("File Not Found"));
* @endcode
*/
#define COH_THROW(E) COH_NO_RETURN_STMT( \
    coherence::lang::coh_throw((E), __FILE__, __LINE__, COH_CURRENT_FUNCTION))

/**
* This macro will take the set of streamable contents and convert them to a
* string which will represent the message of the exception being thrown.
*
* @param E         the name of the exception that will be thrown
* @param CONTENTS  the streamable contents of the message to use when creating
*                  the exception above.
*
* Note: This Macro only supports Exceptions that have a constructor that takes
* a single message parameter.
* Usage example:
* @code
* COH_THROW_STREAM(IOException, "File: " << hsFile << " not found.");
* @endcode
* @see COH_THROW
*/
#define COH_THROW_STREAM(E, CONTENTS) \
    COH_THROW (E::create(String::View(COH_TO_STRING(CONTENTS))));

// ----- free functions -----------------------------------------------------

/**
* This helper method is used by COH_THROW to raise the exception, supplying
* the stack at which it was called.  If the exception is referenced via a
* handle the exception's stack will be set prior to being throw.  The function
* is marked with compiler specific (no return) statements, so that any calls
* to it will suppress the corresponding compiler warnings.
*
* @param ohE         the instance of an exception object to throw
* @param vsFile      the current file
* @param nLine       the current line
* @param vsFunction  the current function
*/
COH_EXPORT COH_NO_RETURN_PRE void coh_throw(Exception::Holder ohE,
        String::View vsFile, int32_t nLine, String::View vsFunction)
            COH_NO_RETURN_POST;

COH_CLOSE_NAMESPACE2

#endif // COH_EXCEPTION_HPP
