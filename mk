#!/bin/sh
#
# See the snapcmakemodules project for details about this script
#     https://github.com/m2osw/snapcmakemodules
TYPE=Debug
# TODO: detect --release and --sanitize and set TYPE accordingly

PROJECT_TEST_PATH="../../BUILD/${TYPE}/contrib/libexcept/tests"
FULL_PROJECT_TEST_PATH=`cd ${PROJECT_TEST_PATH}; pwd`
export PROJECT_TEST_ARGS="--verify-file-inheritance ${FULL_PROJECT_TEST_PATH}"

if test -x ../../cmake/scripts/mk
then
	../../cmake/scripts/mk $*
else
	echo "error: could not locate the cmake mk script"
	exit 1
fi

