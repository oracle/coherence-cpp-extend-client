/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
#ifndef COH_RUN_AS_BLOCK_HPP
#define COH_RUN_AS_BLOCK_HPP

#include "coherence/lang.ns"

#include "coherence/security/auth/Subject.hpp"

COH_OPEN_NAMESPACE2(coherence,security)

COH_OPEN_NAMESPACE(auth)
    class Subject;
COH_CLOSE_NAMESPACE

using coherence::security::auth::Subject;

/**
* The RunAsBlock class allows for easy creation of scoped authorization code
* blocks based on a Subject. The RunAsBlock object will
* push the Subject onto a thread-local "current subject" upon entering the
* block, and pop the subject upon exiting the code block.
*
* A more friendly form is to use the COH_RUN_AS macro. Example usage:
*
* COH_RUN_AS(vSubject)
*   {
*   // code here will considered to be run by the supplied subject
*   // ...
*   // ...
*   } // subject will be reverted
*
* @author mf 2008.08.22
*/
class COH_EXPORT RunAsBlock
    {
    // ----- constructors ---------------------------------------------------

    public:
        /**
        * Construct a RunAsBlock object.
        *
        * This will automatically set the thread's Subject.
        */
        RunAsBlock(TypedHandle<const Subject> vSubject);

        /**
        * Copy constructor, for use by COH_RUN_AS macro.
        *
        * The new block takes over the ownership of the pop.
        */
        RunAsBlock(const RunAsBlock& that);

        /**
        * Destroy a RunAsBlock object.
        *
        * This will automatically pop the Subject.
        */
        ~RunAsBlock();


    // ----- operators ------------------------------------------------------

    public:
        /*
        * Boolean conversion for use in COH_RUN_AS macro.
        *
        * @return false always
        */
        operator bool() const;

    private:
        /**
        * Blocked assignment operator.
        */
        const RunAsBlock& operator=(const RunAsBlock&);

        /**
        * Blocked dynamic allocation.
        */
        static void* operator new(size_t);


    // ----- data members ---------------------------------------------------

    private:
        /**
        * Subject associated with the block.
        */
        mutable TypedHandle<const Subject> m_vSubject; // on stack
    };

COH_CLOSE_NAMESPACE2


/**
* Macro for making more readable run-as code blocks See the documentation of
* RunAsBlock for a usage example.
*
* @see coherence::security::RunAsBlock
*/
#define COH_RUN_AS(VSUBJECT) \
    if (coherence::security::RunAsBlock COH_UNIQUE_IDENTIFIER(_coh_runas_) \
        = coherence::security::RunAsBlock(VSUBJECT)) \
        { \
        COH_THROW(coherence::lang::IllegalStateException::create()); \
        } \
    else

#endif // COH_RUN_AS_BLOCK_HPP
