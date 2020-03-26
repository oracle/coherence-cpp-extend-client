rem
rem  Copyright (c) 2000, 2020, Oracle and/or its affiliates.
rem
rem  Licensed under the Universal Permissive License v 1.0 as shown at
rem  http://oss.oracle.com/licenses/upl.
rem

@echo off

:: This script sets all environment variables necessary to build Oracle
:: Coherence for C++ on Windows.

:: Note: This script MUST be run from a Visual Studio command prompt.

:: Usage : "cfgwindows [-reset]"

:: This script is responsible for the following environment variables:
::
::     DEV_ROOT      e.g. C:\dev\main.cpp
::     arch          e.g. x64
::     ANT_HOME      e.g. C:\dev\main.cpp\tools\common\ant
::     CLASSPATH     e.g.
::     LINKER_HOME   e.g.
::     MSVC_VER      e.g. vs2013
::     PATH          e.g. %ANT_HOME%\bin;%PATH%
::     PERL5LIB      e.g.
::
::     _arch         saved arch
::     _ANT_HOME     saved ANT_HOME
::     _CLASSPATH    saved CLASSPATH
::     _LINKER_HOME  saved LINKER_HOME
::     _MSVC_VER     saved MSVC_VER
::     _PATH         saved PATH
::     _PERL5LIB     saved PERL5LIB

:: --------------------------------------------------------------------------------------------
:: Reset the build environment if the "-reset" flag was passed
:: --------------------------------------------------------------------------------------------
if "%1"=="-reset" (
  if "%DEV_ROOT%"=="" (
    echo Build environment already reset.
    goto exit_err
  )

  set "ANT_HOME=%_ANT_HOME%"
  set "arch=%_arch%"
  set "CLASSPATH=%_CLASSPATH%"
  set "LINKER_HOME=%LINKER_HOME%"
  set "MSVC_VER=%_MSVC_VER%"
  set "PATH=%_PATH%"
  set "PERL5LIB=%_PERL5LIB%"

  set DEV_ROOT=
  set _arch=
  set _ANT_HOME=
  set _CLASSPATH=
  set _LINKER_HOME=
  set _MSVC_VER=
  set _PATH=
  set _PERL5LIB=

  echo Build environment reset.
  goto exit
)

:: --------------------------------------------------------------------------------------------
:: Determine the root of the dev tree
:: --------------------------------------------------------------------------------------------
if not "%DEV_ROOT%"=="" (
  echo Build environment already set.
  goto exit_err
)
for %%i in ("%~dp0..") do @set DEV_ROOT=%%~fni
echo DEV_ROOT     = %DEV_ROOT%

:: --------------------------------------------------------------------------------------------
:: Backup environment variables
:: --------------------------------------------------------------------------------------------
  set "_ANT_HOME=%ANT_HOME%"
  set "_arch=%arch%"
  set "_CLASSPATH=%CLASSPATH%"
  set "_LINKER_HOME=%LINKER_HOME%"
  set "_MSVC_VER=%MSVC_VER%"
  set "_PATH=%PATH%"
  set "_PERL5LIB=%PERL5LIB%"

:: --------------------------------------------------------------------------------------------
:: Set ANT_HOME
:: --------------------------------------------------------------------------------------------
set "ANT_HOME=%DEV_ROOT%\tools\internal\common\ant"

:: --------------------------------------------------------------------------------------------
:: Determine architecture
::
:: VS2017 uses "x64" other versions use "X64"
:: --------------------------------------------------------------------------------------------
if "%PLATFORM%"=="X64" (
  set arch=x64
) else if "%PLATFORM%"=="x64" (
  set arch=x64
) else (
  set arch=x86
)

:: --------------------------------------------------------------------------------------------
:: Set CLASSPATH
:: --------------------------------------------------------------------------------------------
set CLASSPATH=

:: --------------------------------------------------------------------------------------------
:: Set PERL5LIB
:: --------------------------------------------------------------------------------------------
set PERL5LIB=

:: --------------------------------------------------------------------------------------------
:: Determine MSVC version
:: --------------------------------------------------------------------------------------------
if "x%VisualStudioVersion%"=="x" (
  echo Environment variable VisualStudioVersion not set.
  echo Assuming VS 2010...
  set MSVC_VER=2010
) else if "%VisualStudioVersion%"=="16.0" (
  set MSVC_VER=2019
) else if "%VisualStudioVersion%"=="15.0" (
  set MSVC_VER=2017
) else if "%VisualStudioVersion%"=="14.0" (
  set MSVC_VER=2015
) else if "%VisualStudioVersion%"=="12.0" (
  set MSVC_VER=2013
) else if "%VisualStudioVersion%"=="11.0" (
  set MSVC_VER=2012
) else (
  echo Unknown or unsupported Visual Studio Version.
  echo Environment variable VisualStudioVersion=%VisualStudioVersion%.
  goto exit_err
)

:: --------------------------------------------------------------------------------------------
:: Use the 64 bit link.exe for VS2013 and 2012 32 bit builds
:: --------------------------------------------------------------------------------------------
if "%arch%"=="x86" (
  if "%MSVC_VER%"=="2013" (
    set "LINKER_HOME=%VCINSTALLDIR%bin\amd64"
  ) else if "%MSVC_VER%"=="2012" (
    set "LINKER_HOME=%VCINSTALLDIR%bin\amd64"
  )
)

:: --------------------------------------------------------------------------------------------
:: Process override file if present
:: --------------------------------------------------------------------------------------------
if exist c:\coh_cpp_override.cmd (
 echo Executing c:\coh_cpp_override.cmd
 call c:\coh_cpp_override.cmd
)

:: --------------------------------------------------------------------------------------------
:: Set JAVA_HOME
:: --------------------------------------------------------------------------------------------
if defined JAVA_HOME (
  :: Strip quotes from JAVA_HOME environment variable if present
  set JAVA_HOME=%JAVA_HOME:"=%
) else (
 echo JAVA_HOME not set.
 goto exit_err
)

:: --------------------------------------------------------------------------------------------
:: Add ANT_HOME to PATH
:: --------------------------------------------------------------------------------------------
set "PATH=%ANT_HOME%\bin;%PATH%"

:: --------------------------------------------------------------------------------------------
:: Display settings
:: --------------------------------------------------------------------------------------------
echo arch         = %arch%
echo ANT_HOME     = %ANT_HOME%
echo CLASSPATH    = %CLASSPATH%
echo INCLUDE      = %INCLUDE%
echo JAVA_HOME    = %JAVA_HOME%
"%JAVA_HOME%\bin\java" -version
echo LIB          = %LIB%
echo LINKER_HOME  = %LINKER_HOME%
echo MSVC_VER     = %MSVC_VER%
echo PATH         = %PATH%
echo PERL5LIB     = %PERL5LIB%

:: --------------------------------------------------------------------------------------------
:: Clean exit
:: --------------------------------------------------------------------------------------------
:exit
exit /b 0

:: --------------------------------------------------------------------------------------------
:: Exit with error.  Note the comspec command needs to be at the end of the file to work
:: --------------------------------------------------------------------------------------------
:exit_err
@%COMSPEC% /C exit 1 >nul
