/*
 * Copyright (c) 2000, 2020, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * http://oss.oracle.com/licenses/upl.
 */
/**
* The Coherence for C++ application launcher.
*
* It provides roughly the same functionality as "java.exe", namely it allows
* for execution of a "public static void main" method from a registered
* class.  Specifically the class must be registered with the SystemClassLoader
* as an ExecutableClass.  The benefits of using this approach for starting
* applications over the traditional C++ method are:
* - system properties can be specified on the command line
* - multiple "utility" applications can be bundled in one DLL
* - exceptions thrown from "main" can be nicely logged
*
* This application performs the following tasks:
* - store command line system properties as process local environment variables
* - load dynamic libraries
* - invoke a well known function in the Coherence library.
*
* As part of loading the DLLs they are statically initialized which triggers
* class registration.  The loading of an application library which has been
* linked with the Coherence library, is what actually triggers the loading
* of Coherence.  This way the launcher application is able to set environment
* variables prior to the Coherence library being loaded.
*
* This application is not linked against the Coherence library allowing it
* to work with whatever version the application library had been linked
* against.  This allows for a single version to support both debug and release
* versions of the Coherence library.
*
* @author mf 2008.10.03
*
* @since Coherence 3.5
*/

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#ifdef _WIN32
#  include <windows.h>
#  define LIB_PREFIX  ""
#  define LIB_SUFFIX  ".dll"
#  define DLIB        HMODULE
#  define DLSYM       GetProcAddress
#  define DLOPEN(L)   LoadLibrary(L)
#  define DLCLOSE(P)  !FreeLibrary(P)
#  define strncasecmp _strnicmp
#else // UNIX
#  include <dlfcn.h>
#  include <strings.h>
#  define LIB_PREFIX "lib"
#  ifdef __APPLE__ // OS X
#    define LIB_SUFFIX ".dylib"
#  else // Linux, Solaris, ...
#    define LIB_SUFFIX ".so"
#  endif
#  define DLIB       void*
#  define DLSYM      dlsym
#  define DLOPEN(L)  dlopen(L, RTLD_NOW | RTLD_GLOBAL)
#  define DLCLOSE(P) dlclose(P)
#endif

using std::string;

/**
* Function pointer definition for "main", see ExecutableClass.cpp
*/
typedef int (*main_func)(int, char**);

/**
* Helper to cast cast from void* to function pointer without generating a
* warning.
*
* http://www.opengroup.org/onlinepubs/009695399/functions/dlsym.html
*/
union uRawFunc
    {
    void*     pRaw;
    main_func pMain;
    };

/**
* Load the specified dynamic library.
*
* @param sLib      the library to load
* @param pCohMain  the coherence entrypoint, output
* @param fError    if true, fail on error
*
* @return the library handle or NULL on failure
*/
DLIB load(const string sLib, main_func& pCohMain, bool fError)
    {
    DLIB pLib = DLOPEN(sLib.c_str());
    while (pLib == NULL)
        {
        // test if the supplied library needs to have prefix/suffix added
        if (sLib.find('/') == string::npos && sLib.find('\\') == string::npos)
            {
            static const string sLibPrefix = LIB_PREFIX;
            static const string sLibSuffix = LIB_SUFFIX;
            static       size_t cchPrefix  = sLibPrefix.length();
            static       size_t cchSuffix  = sLibSuffix.length();
                         size_t cchLib     = sLib.length();

            // Use C string library as C++ strings class does not have a case insensitive compare
            bool fPrefix = strncasecmp(sLibPrefix.c_str(), sLib.c_str(), cchPrefix) != 0;
            bool fSuffix = cchLib <= cchSuffix ||
                strncasecmp(sLibSuffix.c_str(), sLib.substr(cchLib - cchSuffix, cchSuffix).c_str(), cchSuffix) != 0;

            if (fPrefix || fSuffix)
                {
                string sLibNew;

                if (fPrefix)
                    {
                    sLibNew += sLibPrefix;
                    }

                sLibNew += sLib;

                if (fSuffix)
                    {
                    sLibNew += sLibSuffix;
                    }

                pLib = DLOPEN(sLibNew.c_str());
                if (pLib != NULL)
                    {
                    break;
                    }
                }
            }

        if (fError)
            {
            std::cout << "error: unable to load library \"" << sLib << '\"' << std::endl;
            exit(1);
            }
        return NULL;
        }

    uRawFunc uSymb;
    uSymb.pRaw = DLSYM(pLib, "coh_execute_class");
    if (uSymb.pRaw != NULL)
        {
        if (pCohMain != NULL && uSymb.pMain != pCohMain)
            {
            printf("error multiply defined coherence symbols\n");
            exit(1);
            }
        pCohMain = uSymb.pMain;
        }

    return pLib;
    }

/**
* Load the coherence library, and return its entrypoint.
*
* @param pCohMain   the coherence entrypoint, output
*/
DLIB loadCoherence(main_func& pCohMain)
    {
    // no libs specified, default to coherence debug
    DLIB pLib = load("coherence-debug", pCohMain, false);
    if (pLib == NULL)
        {
        // not found, try coherence release
        pLib = load("coherence", pCohMain, false);
        }

    if (pCohMain == NULL)
        {
        printf("error: Coherence library not found.\n");
        exit(1);
        }
    return pLib;
    }

/**
* Print the Coherence version.
*
* @param pCohMain   the coherence entrypoint
*/
void printVersion(main_func pCohMain)
    {
    if (pCohMain == NULL)
        {
        loadCoherence(pCohMain);
        }

    const char* arg = "coherence::lang::System";
    (*pCohMain)(1, (char**) &arg);
    }

/**
* Print the help message.
*/
void printHelp()
    {
    printf("usage: sanka [-options] <native class> [args...]\n\n");
    printf("available options include:\n");
    printf("\t-l <native library list>  dynamic libraries to load, separated by : or ;\n");
    printf("\t-D<property>=<value>      set a system property\n");
    printf("\t-version                  print the Coherence version\n");
    printf("\t-?                        print this help message\n");
    printf("\t<native class>            full class name to execute e.g. coherence::net::CacheFactory\n");
    }

/**
* Process entrypoint.
*/
int main(int argc, char** argv)
    {
    bool      fVersion  = false;
    bool      fRepeat   = false;
    main_func pCohMain  = NULL;
    int       nResult   = 1;
    int       cLib = 0;
    DLIB      apLib[256];

    // parse command line arguments
    do
        {
        int i = 1;
        for (; i < argc && argv[i][0] == '-'; ++i)
            {
            string sArg = argv[i];

            if (sArg == "-?")
                {
                printHelp();
                return 0;
                }
            else if (sArg == "-version")
                {
                fVersion = true;
                }
            else if (sArg == "-l")
                {
                // load libs
                if (i == argc - 1)
                    {
                    printf("error: no library specified\n");
                    printHelp();
                    return 1;
                    }

                string sLibs        = argv[++i];
                size_t cLibStartPos = 0;
                for (size_t j = 0; j < sLibs.length(); ++j)
                    {
                    if (sLibs[j] == ':' || sLibs[j] == ';')
                        {
                        apLib[cLib++] = load(sLibs.substr(cLibStartPos, j - cLibStartPos), pCohMain, true);
                        cLibStartPos = j + 1;
                        }
                    }
                if (cLibStartPos < sLibs.length())
                    {
                    apLib[cLib++] = load(cLibStartPos == 0 ? sLibs : sLibs.substr(cLibStartPos),
                            pCohMain, true);
                    }
                }
            else if (sArg == "-Xrepeat") // undocumented
                {
                // run this program in an endless loop
                fRepeat = true;
                if (pCohMain != NULL)
                    {
                    static int cReps = 0;
                    // allow testing of library reloading, for this to work
                    // -Xrepeat must precede the library list, and only one
                    // coherence aware library should be used
                    while (cLib-- > 0)
                        {
                        int nError = DLCLOSE(apLib[cLib]);
                        if (nError != 0)
                            {
                            printf("library unload error: %d\n", nError);
                            }
                        }
                    cLib     = 0;
                    pCohMain = NULL;
                    printf("iteration %d\n", ++cReps);
                    }
                }
            else if (sArg.length() > 2 && sArg.substr(0, 2) == "-D")
                {
                // system property
    #ifdef _WIN32
                _putenv(sArg.substr(2).c_str());
    #else
                string sName = sArg.substr(2);
                string sVal  = "";
                for (size_t j = 2; j < sArg.length(); ++j)
                    {
                    if (sArg[j] == '=' && j < sArg.length() - 1)
                        {
                        sName = sArg.substr(2, j - 2);
                        sVal  = sArg.substr(j + 1);
                        }
                    }
                if (sName.length() > 0)
                    {
                    setenv(sName.c_str(), sVal.c_str(), true);
                    }
    #endif
                }
            else // unknown option
                {
                std::cout << "error: unknown option " << sArg << std::endl;
                printHelp();
                return 1;
                }
            }

        if (i < argc)
            {
            // ensure Coherence library has been loaded
            if (pCohMain == NULL)
               {
               apLib[cLib++] = loadCoherence(pCohMain);
               }

            if (fVersion)
                {
                printVersion(pCohMain);
                }

            nResult = (*pCohMain)(argc - i, argv + i);
            }
        else if (fVersion)
            {
            printVersion(pCohMain);
            }
        else
            {
            printHelp();
            }
        }
    while (fRepeat);

    return nResult;
    }
