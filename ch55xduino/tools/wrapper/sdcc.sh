#!/bin/bash

# wrapper around SDCC to let the external interface look more gcc-alike
#
# Differences:
# - if a .cpp files is given as input temporarly copy it to .c and compile it
#   as a c file. This will break the dependency check, as it expects the
#   full original filename, but as this happens only for the original .ino
#   file it is not a big loss.
# - generate .rel files, but copy them as .o files as well to satisfy the
#   dependency checker on following builds


VERBOSE=0
USE_COLOR=0
if [ "$1" == "-v" ]; then
	VERBOSE=1;
	shift
elif [ "$1" == "-vv" ]; then
	VERBOSE=2
#	USE_COLOR=1
	set -x
	shift
fi


if [ $USE_COLOR -gt 0 ]; then
# ANSI color codes to beautify the output:
BLACK='\033[0;30m'
RED='\033[0;31m'
GREEN='\033[0;32m'
ORANGE='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
LGRAY='\033[0;37m'
DGRAY='\033[1;30m'
LRED='\033[1;31m'
LGREEN='\033[1;32m'
YELLOW='\033[1;33m'
LBLUE='\033[1;34m'
LPURPLE='\033[1;35m'
LCYAN='\033[1;36m'
WHITE='\033[1;37m'
OFF='\033[0m'
fi

# check if cp is in the path using 'command -v' (a builtin POSIX function)
if ! command -v cp > /dev/null; then
	# Ok, this means we are on a Windows system and we have to find a
	# way to access cp and rm in ../win. A simple 'cd ../win' or
	# '../win/cp' does't work, as the current working directory is still
	# the Arduino binary directory.
	#
	# This looks ok, but it doesn't work on some Windows systems:
	# (No idea why)
	# PATH="${0%/wrapper/*}"/win:$PATH
	#
	# This is technically wrong, but surprisingly it works with Windows:
	# cd $0/../..
	# PATH=$(pwd)/win:$PATH
	#
	# Use cd/pwd as a replacement for 'realpath' using only builtins.
	# It has the positive side effect of converting from Windows to Unix
	# path syntax avoiding all these backslash issues.
	cd "${0%/wrapper/*}"
	PATH=$(pwd)/win:$PATH
fi

# echo the full command line in cyan:
if [ $VERBOSE -gt 0 ]; then
	>&2 echo -ne "${CYAN}"
	>&2 echo -n  "${@}"
	>&2 echo -e  "${OFF}"
fi

# echo the mark id in green and the compiler call in white:
SDCC=$1
SRC=$2
OBJ=$3
REL=${OBJ%.o}.rel
MARK=$4
shift 4

if [ $VERBOSE -gt 0 ]; then
	>&2 echo -ne "${GREEN}Mark $MARK:${OFF}"
	>&2 echo "$SDCC" "$@" "$SRC" -o "$OBJ"
fi

case "$SRC" in
	*.cpp)
		# rename .cpp to .c and compile
		if [ $VERBOSE -gt 0 ]; then
			>&2 echo -e "${RED}cpp found${OFF}";
		fi
		CSRC="${SRC%pp}"
		(
			# add a reference to main to pull in main.c
			echo "void main(void); void (*dummy_variable) () = main;"
                	cat "$SRC"
                ) > "$CSRC"
#		cp -a "$SRC" "$CSRC"
		"$SDCC" "$@" "$CSRC" -o "$OBJ"
		ERR=$?
		rm -f "$CSRC"
		;;
	*.c)
		# compile a .c file
		"$SDCC" "$@" "$SRC" -o "$OBJ"
		ERR=$?
		;;
esac

# Deqing: For some reason my version of SDCC don't generate rel file, it creates o file directly, so I modified the SDCC command below
# copy the generated .rel files as an .o file to avoid recompiling the next time
# if OBJ is a .o file we copy back
if [[ ${OBJ%.o} != $OBJ ]]
then
	cp -a "${OBJ}" "${REL}"
fi

# propagate the sdcc exit code
exit $ERR
