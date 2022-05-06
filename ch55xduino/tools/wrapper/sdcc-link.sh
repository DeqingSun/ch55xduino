#!/bin/bash

# usage: sdcc-link [options] [.lib and .rel files] re5 [other flags and files]
#
# possible script options (options in this order only):
# -v:	verbose
# -d:	debug mode (more verbose), includes coloring the output
# -c:	color the output


### local functions ######################################################

# verbose print
#
# usage: vprint min_level msg
#
# prints a message if the verbosity level is equal or higher than the required
# min_level
#
vprint ()
{
	local level=$1

	shift
	if [ $VERBOSE -ge $level ]; then
		echo -e "$@"
	fi
}


# parse the script options
#
# This is very crude. The options are allowed only as the very first argments
# and they are required to be used in exactly this order.

VERBOSE=0
USE_COLOR=0
if [ "$1" == "-v" ]; then
	VERBOSE=1;
	shift
elif [ "$1" == "-d" ]; then
	VERBOSE=2
	USE_COLOR=1
	set -x
	shift
fi
if [ "$1" == "-c" ]; then
	USE_COLOR=1;
	shift
fi


# define color codes

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


SDCC="$1"
shift

# echo the full command line in cyan on stderr:
>&2 vprint 1 "${CYAN}${@}${OFF}"


# The arduino system insists on a *.a file for a library, but sdar requires
# them to be named *.lib.
#
# Workaround: copy all *.lib files into *.a files.
#
# Iterate over all positional parameters with a for loop.
# The pattern match for filename is easy for bash and dash, but busybox ash
# requires the use of the 'expr' command:
#
# bash, dash: if [[ "$FILE" == *.a ]]; then
# ash uses 'expr': expr "$FILE" : ".*\.a$"; then
#
# This is all pure POSIX, it works for bash, dash and busybox ash
vprint 2 "copy *.a to *.lib"
for FILE; do
	vprint 2 "- checking parameter '$FILE'"
	# using expr works for bash, dash, busybox ash
	if expr "$FILE" : ".*\.a$" > /dev/null; then
		NEW=${FILE%.a}.lib
		vprint 1 "copy $FILE to $NEW"
		cp -a "$FILE" "$NEW"
	fi
done


# replace *.o with *.rel and *.a with *.lib
#
# On bash this is a simple pattern substituiton:
# set -- "${@/.o/.rel}"
# set -- "${@/.a/.lib}"
#
# Unfortunatly, this does not work with dash or ash. dash does not support
# pattern substituition in general. busybox ash does not support arrays and
# shortens the arg list to the first argument, deleting all the rest.
#
# As a workaround we combine the argument list into a single string. By
# using TAB as a field separator we can even deal with spaces, backspaces
# and colons in file names.

# use tab as field separator
IFS=$'\t'


# combine all arguments into a single string with field separator and add a
# TAB at the end. This allows the pattern below to match the last argument
# as well.
vprint 2 "Combine all arguments into a single string"
line="$*	"

# do the filename replacements: (bash and ash, not dash)
# Needs a double slash to replace all occurencies, not only the first one.
#line=${line//.o/.rel}
#line=${line//.a/.lib}
# replace all the occurencies just before a field separator
vprint 2 "- before substitution: $line"
line="${line//.o	/.rel	}"
line="${line//.a	/.lib	}"
vprint 2 "- after substitution: $line"


#put rel files with optionalLink name into a lib to do dead code removal
outPath=""
for FILE in $line; do
	if expr "$FILE" : ".*\.elf$" > /dev/null; then
		outPath="$(dirname "${FILE}")"
	fi
done
optionalLinkLib="$outPath/optionalLink.lib"

lineRel=""
lineExtractRel=""
optionalLinkFound="false"

for FILE in $line; do
	if expr "$FILE" : ".*optionalLink.*\.rel$" > /dev/null; then
		lineExtractRel="$lineExtractRel	$FILE"
		if [[ "$optionalLinkFound" == "false" ]]; then
			lineRel="$lineRel	$optionalLinkLib"
			optionalLinkFound="true"
		fi
	else
		lineRel="$lineRel	$FILE"
	fi
done

if [[ "$optionalLinkFound" == "true" ]]; then
	if [ -f "$optionalLinkLib" ] ; then
	    rm "$optionalLinkLib"
	fi
	
	SDAR="$(dirname "${SDCC}")/sdar"
	for FILE in $lineExtractRel; do
		"$SDAR"	"rcs" "$optionalLinkLib" "$FILE"
	done
fi

vprint 1 "cmd: ${ORANGE}$SDCC $lineRel${OFF}"
"$SDCC" $lineRel

#check if CSEG is even
MAPFILE=${FILE%.*}.map
if [ -f ${MAPFILE} ]; then
    #only get 1st match
    CSEG_ADDR_STR="$(grep -o '^CSEG[ ]\+[0-9A-F]\+' ${MAPFILE} | head -1)"
    CSEG_ADDR_HEX_VAL="$(echo ${CSEG_ADDR_STR} | grep -o '[^ ]*$')"
    CSEG_ADDR_DEC_VAL="$(printf '%d' 0x${CSEG_ADDR_HEX_VAL})"
    if [ $((CSEG_ADDR_DEC_VAL%2)) -eq 1 ]; then
        >&2 echo "Warning: CSEG starts at odd address: ${CSEG_ADDR_HEX_VAL}, and it may cause timing issue"
    fi
fi

# propagate the sdcc exit code
exit $?
