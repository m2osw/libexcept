#!/bin/sh
#
# See the snapcmakemodules project for details about this script
#     https://github.com/m2osw/snapcmakemodules

# We need the type in order to determine the destination folder...
# TODO: try to find a better way to handle this case
TYPE=Debug
for arg
do
	case "$arg" in
	"--debug"|"-g")
		shift
		TYPE="Debug"
		;;

	"--release"|"-r")
		shift
		TYPE="Release"
		;;

	"--sanitize"|"-s")
		shift
		TYPE="Sanitize"
		;;
	esac
done

PROJECT_TEST_PATH="../../BUILD/${TYPE}/contrib/libexcept"
FULL_PROJECT_TEST_PATH=`cd ${PROJECT_TEST_PATH}; pwd`
export PROJECT_TEST_ARGS="--verify-file-inheritance ${FULL_PROJECT_TEST_PATH}/tools"

if test -x ../../cmake/scripts/mk
then
	../../cmake/scripts/mk $*
else
	echo "error: could not locate the cmake mk script"
	exit 1
fi

