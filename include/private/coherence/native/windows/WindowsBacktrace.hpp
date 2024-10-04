/*
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 *
 * Licensed under the Universal Permissive License v 1.0 as shown at
 * https://oss.oracle.com/licenses/upl.
 */
#ifndef COH_WINDOWS_BACKTRACE_HPP
#define COH_WINDOWS_BACKTRACE_HPP

#include "private/coherence/native/NativeBackTrace.hpp"

#include "coherence/util/ArrayList.hpp"

#include "private/coherence/native/NativeABI.hpp"
#include "private/coherence/native/NativeStackElement.hpp"

#include <windows.h>

// Visual Studio 2015 throws C4091 on ImageHlp.h
__pragma(warning(push))
__pragma(warning(disable : 4091))
#include <ImageHlp.h>
__pragma(warning(pop))

COH_OPEN_NAMESPACE2(coherence,native)

using coherence::util::ArrayList;

#pragma comment(lib, "imagehlp.lib")


// ----- file local helpers -------------------------------------------------

namespace
    {
    typedef void (__stdcall *RTLCAPTURECONTEXT)(PCONTEXT);

    static BOOL              fBacktraceEnabled  = FALSE;
    static HMODULE           hKernel32Dll       = NULL;
    static RTLCAPTURECONTEXT pRtlCaptureContext = NULL;

    /**
    * Initialize backtrace support.
    */
    void initializeBacktrace()
        {
        // COH-31048 - dbghelp symbol support uses a fair amount of memory; backtrace collection
        //             can be disabled for environments where system memory is at a premium
        bool fBackTrace = Boolean::parse(System::getProperty("coherence.backtrace.enabled", "true"));

        if (!fBackTrace)
            {
            fBacktraceEnabled  = FALSE;
            return;
            }

        hKernel32Dll = LoadLibrary(TEXT("kernel32.dll"));
        if (hKernel32Dll != NULL)
            {
            pRtlCaptureContext = (RTLCAPTURECONTEXT) GetProcAddress(hKernel32Dll, TEXT("RtlCaptureContext"));

            if (pRtlCaptureContext != NULL)
                {
                fBacktraceEnabled = TRUE;
                }
            }

        if (!fBacktraceEnabled)
            {
            return;
            }

        // TODO: allow user to turn off this feature, SymInitialize can only
        // be called once per process.
        DWORD symOptions = SymGetOptions();

        symOptions |= SYMOPT_LOAD_LINES;
        symOptions &= ~SYMOPT_UNDNAME;
        SymSetOptions(symOptions);

        // TODO: should we define our own variable for this?
        String::View vsPath = System::getProperty("PATH");
        if (NULL == vsPath)
            {
            SymInitialize(GetCurrentProcess(), NULL, TRUE);
            }
        else
            {
            // stupid function wants a char* not const char*, can't trust
            // that it won't modify the string
            size32_t cch     = vsPath->getOctets()->length;
            char*    achPath = new char[cch];
            strncpy_s(achPath, cch, vsPath->getCString(), cch);
            SymInitialize(GetCurrentProcess(), achPath, TRUE);
            delete[] achPath;
            }

        // TODO: add SymCleanup call on DLL unload
        }
    COH_STATIC_INIT(initializeBacktrace());

    /**
    * Obtain source level information as
    *     filename:linenumber
    * or
    *     modulename!address
    *
    * @param uAddr        symbol address
    *
    * @return the source info String
    */
    String::Handle SourceInfo(DWORD64 uAddr)
        {
        DWORD             dwDisp;
        IMAGEHLP_LINE64   lineInfo;
        IMAGEHLP_MODULE64 moduleInfo;
        HANDLE            hProcess = GetCurrentProcess();

        ZeroMemory(&lineInfo, sizeof(lineInfo));
        lineInfo.SizeOfStruct = sizeof(lineInfo);

        if (SymGetLineFromAddr64(hProcess, uAddr, &dwDisp, &lineInfo))
            {
            return COH_TO_STRING(lineInfo.FileName << ':' << lineInfo.LineNumber);
            }

        // Failed to obtain source info, try module name instead
        ZeroMemory(&moduleInfo, sizeof(moduleInfo));
        moduleInfo.SizeOfStruct = sizeof(moduleInfo);

        if (SymGetModuleInfo64(hProcess, uAddr, &moduleInfo))
            {
            return COH_TO_STRING(moduleInfo.ModuleName << '!' << uAddr);
            }
        return COH_TO_STRING('!' << uAddr);
        }

    /**
    * Obtain symbol information
    *
    * @param uAddr        symbol address
    * @param uStackAddr   stackframe address
    *
    * @return the symbol info String
    */
    String::Handle SymbolInfo(DWORD64 uAddr, DWORD64 uStackAddr)
        {
        const size32_t     cb       = 10000; // REVIEW: what is 10000 based on?
        DWORD64            dwDisp   = 0;
        HANDLE             hProcess = GetCurrentProcess();
        PIMAGEHLP_SYMBOL64 pSymbol  = (PIMAGEHLP_SYMBOL64) GlobalAlloc(GMEM_FIXED, cb);

        ZeroMemory(pSymbol, cb);

        pSymbol->SizeOfStruct  = cb;
        pSymbol->MaxNameLength = cb - sizeof(IMAGEHLP_SYMBOL64);

        const size32_t cch = 1024; // REVIEW: what is 1024 based on?
        char achInfo[cch];
        achInfo[0] = 0;

        if (SymGetSymFromAddr64(hProcess, uAddr, &dwDisp, pSymbol))
            {
            UnDecorateSymbolName(pSymbol->Name, achInfo, cch,
                UNDNAME_NO_MS_KEYWORDS | UNDNAME_COMPLETE | UNDNAME_NO_THISTYPE |
                UNDNAME_NO_SPECIAL_SYMS | UNDNAME_NO_MEMBER_TYPE |
                UNDNAME_NO_ACCESS_SPECIFIERS);
            }

        GlobalFree(pSymbol);

        return achInfo[0] ? String::create(achInfo) : StringHelper::getEmptyString();
        }

    /**
    * Object to synchronize DbgHelp methods.
    */
    static Object::View getDbgHelpLock()
        {
        static FinalView<Object> vo(System::common(), Object::create());
        return vo;
        }
    COH_STATIC_INIT(getDbgHelpLock());
    }


// ----- NativeBacktrace static interface -----------------------------------

ObjectArray::Handle NativeBacktrace::getStackTrace(size32_t cTrim)
    {
    static bool fTrim = Boolean::parse(System::getProperty
        ("coherence.threaddump.trim", "true")); // for diagnostics

    ArrayList::Handle haFrames = ArrayList::create();

    if(fBacktraceEnabled)
        {
        COH_SYNCHRONIZED(getDbgHelpLock())
            {
            HANDLE hThread  = GetCurrentThread();
            HANDLE hProcess = GetCurrentProcess();

            CONTEXT      context;
            STACKFRAME64 callStack;
            DWORD        cMachineType;

            ZeroMemory(&context, sizeof(context));
            context.ContextFlags = CONTEXT_FULL;

            //RtlCaptureContext(&context);
            pRtlCaptureContext(&context);
            ZeroMemory(&callStack, sizeof(callStack));

#ifdef COH_OS_WIN64
            cMachineType               = IMAGE_FILE_MACHINE_AMD64;
            callStack.AddrPC.Offset    = context.Rip;
            callStack.AddrStack.Offset = context.Rsp;
            callStack.AddrFrame.Offset = context.Rbp;
#else
            cMachineType               = IMAGE_FILE_MACHINE_I386;
            callStack.AddrPC.Offset    = context.Eip;
            callStack.AddrStack.Offset = context.Esp;
            callStack.AddrFrame.Offset = context.Ebp;
#endif
            callStack.AddrPC.Mode      = AddrModeFlat;
            callStack.AddrStack.Mode   = AddrModeFlat;
            callStack.AddrFrame.Mode   = AddrModeFlat;

            for (size32_t i = 0; ; ++i)
                {
                BOOL fResult = StackWalk64(cMachineType, hProcess,
                    hThread, &callStack, &context, NULL, SymFunctionTableAccess64,
                    SymGetModuleBase64, NULL);

                if (!fResult || callStack.AddrFrame.Offset == 0)
                    {
                    break;
                    }

                if (i >= cTrim || !fTrim)
                    {
                    // TODO: add funtion name.  At that point NativeStackElement
                    // is insufficient, and ExceptionStackElement is a better
                    // (though poorly named choice).  Probably rename
                    // ExceptionStackElement to something better, and re-use.

                    String::View vsName = SymbolInfo(callStack.AddrPC.Offset,
                            callStack.AddrFrame.Offset);
                    haFrames->add(NativeStackElement::create(
                        SourceInfo(callStack.AddrPC.Offset), vsName));

                    // trim off OS specific bits below coherence created threads
                    if (fTrim && vsName->equals("coherence::lang::Thread::run"))
                        {
                        break;
                        }
                    }
                }
            }
        }

        return haFrames->toArray();
    }

COH_CLOSE_NAMESPACE2

#endif // COH_WINDOWS_BACKTRACE_HPP
