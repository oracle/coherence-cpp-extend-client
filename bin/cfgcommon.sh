#!/bin/bash
#
#  Copyright (c) 2000, 2023, Oracle and/or its affiliates.
#
#  Licensed under the Universal Permissive License v 1.0 as shown at
#  https://oss.oracle.com/licenses/upl.
#
# This script sets all environment variables necessary to build Oracle
# Coherence for C++ on supported Unix platforms.
#
# Command line:
#     . ./cfglocal.sh [-reset]
#
# This script is responsible for the following environment variables:
#
#     DEV_ROOT     e.g. /home/user/dev/main.cpp
#     ANT_HOME     e.g. /home/user/dev/main.cpp/tools/common/ant
#     CC_HOME      e.g.
#     CLASSPATH    e.g.
#     PATH         e.g. $ANT_HOME/bin:$JAVA_HOME/bin:$CC_HOME/bin:$PATH
#
#     _ANT_HOME    saved ANT_HOME
#     _CC_HOME     saved CC_HOME
#     _CLASSPATH   saved CLASSPATH
#     _PATH        saved PATH
#

#
# Reset the build environment if the "-reset" flag was passed
#
if [ "$1" = "-reset" ]; then

  if [ -z $DEV_ROOT ]; then
    echo Build environment already reset.
    return 0
  fi

  if [ -z $_ANT_HOME ]; then
    unset ANT_HOME
  else
    export ANT_HOME=$_ANT_HOME
  fi

  if [ -z $_CC_HOME ]; then
    unset CC_HOME
  else
    export CC_HOME=$_CC_HOME
  fi

  if [ -z "$_CLASSPATH" ]; then
    unset CLASSPATH
  else
    export CLASSPATH=$_CLASSPATH
  fi

  if [ -z "$_PATH" ]; then
    unset PATH
  else
    export PATH=$_PATH
  fi

  unset DEV_ROOT
  unset _ANT_HOME
  unset _CC_HOME
  unset _CLASSPATH
  unset _PATH

  echo Build environment reset.
  return 0
fi

#
# Determine the root of the dev tree
#
if [ ! -z $DEV_ROOT ]; then
  echo Build environment already set.
  return 0
fi
cd $SCRIPTS_DIR/..
DEV_ROOT=`pwd`
cd - > /dev/null

#
# Back up environment variables
#
_ANT_HOME=$ANT_HOME
_CC_HOME=$CC_HOME
_CLASSPATH=$CLASSPATH
_PATH=$PATH

#
# Set ANT_HOME
#
ANT_HOME=$DEV_ROOT/tools/internal/common/ant

#
# Set CC_HOME
#
if [ $(uname) == "SunOS" ]; then
  CC_HOME=/opt/SunProd/studio12u1/p11/sunstudio12.1
  # else - assume the compiler is already in the PATH on other platforms
fi

#
# Set JAVA_HOME
#
if [ ! -d "$JAVA_HOME" ]; then
  if [ $(uname) == "Darwin" ]; then
    # On OS X can query for JDK location
    JAVA_HOME=`/usr/libexec/java_home -v 17 2>&1`
  fi
fi

#
# Unset CLASSPATH
#
unset CLASSPATH

#
# Load overrides for environment variables prior to adding them to PATH
#
if [ -a ${HOME}/coh_cpp_override.sh ]; then
  echo Executing override file ${HOME}/coh_cpp_override.sh
  . ${HOME}/coh_cpp_override.sh
fi

#
# Validate JAVA_HOME
#
if [ ! -d "$JAVA_HOME" ]; then
  echo Error: JAVA_HOME not specified or not a directory
  return 1
fi

#
# Add CC_HOME to PATH
#
if [ -n "$CC_HOME" ]; then
  PATH=$CC_HOME/bin:$PATH
fi

#
# Add ANT_HOME to PATH
#
if [ -n $ANT_HOME ]; then
  PATH=$ANT_HOME/bin:$PATH
fi

#
# Add the RQ executables to the PATH environment variable
#
PATH=$PATH:$DEV_ROOT/tools/internal/wls/infra

#
# Echo Java and Compiler versions
#
echo
echo Build tools:
$JAVA_HOME/bin/java -version
case $(uname) in
  "Darwin")
    if [ -z "$CC_HOME" ]; then
      xcodebuild -version
      g++ --version
    else
      g++ --version
    fi
    ;;
  "Linux")
    g++ --version
    ;;
  "SunOS")
    $CC_HOME/bin/version $CC_HOME/bin/CC
    ;;
esac
echo

#
# Export and echo environment variables
#
echo Environment variables:
export DEV_ROOT
echo "DEV_ROOT  = $DEV_ROOT"
export CC_HOME
echo "CC_HOME   = $CC_HOME"
export JAVA_HOME
echo "JAVA_HOME = $JAVA_HOME"
export ANT_HOME
echo "ANT_HOME  = $ANT_HOME"
export CLASSPATH
echo "CLASSPATH = $CLASSPATH"
export PATH
echo "PATH      = $PATH"
echo
echo Build environment set
