#!/bin/bash

# helper script for automatic releases.
#
# Prints one platform entry for the given file to be added to the
# package_*.json file.


if [ $# -ne 3 ]; then
	echo
    echo "helper script for automatic releases."
    echo
    echo "Prints a full tool entry for the given filename stem to be added to the"
    echo "package_*_index.json file. It will include all OS variants found with the"
    echo "same filename stem and the same version specifier."
    echo
    echo "usage: $0 toolfile-stem toolversion coreversion"
    echo
    echo "Lists info for all files matching the filename pattern"
    echo "[toolsfile-stem]*[version]*"
    echo
    echo "The coreversion is only needed to build the download link."
    echo
    echo "Example: $0 release/sduino-tools 2017-10-21 0.3.1"
    echo "	prints information for all files matching the filename pattern"
    echo "	release/sduino-tools*2017-10-21* and generates download links"
    echo "	for a github release directory download/v0.3.1."
    echo
	exit 1
fi

TRUNK=$1
VERSION=$2
COREVERSION=$3

BASEURL=https://github.com/DeqingSun/ch55xduino/releases/download/${COREVERSION}

### helper functions #####################################################

# format ID information for a file
#
# usage: print_filedata filename
#
print_filedata()
{
	FILENAME=$(basename "$1")
	URL=${BASEURL}/${FILENAME}
	SIZE=$(stat -f%z $1)
	CHKSUM=$(shasum -a 256 $1|cut "-d " -f1)
	cat << EOF
                            "url": "$URL",
                            "archiveFileName": "$FILENAME",
                            "checksum": "SHA-256:$CHKSUM",
                            "size": "$SIZE"
EOF
}


# detect the host system type for the given file
detect_hosttype()
{
	case $1 in
		*amd64-unknown-linux* | *linux64* | *amd64-ubuntu*-linux* )
			HOST="x86_64-pc-linux-gnu"
			;;
		*mingw32*)
			HOST="i686-mingw32"
			;;
		*i386-unknown-linux* | *linux32* )
			HOST="i686-pc-linux-gnu"
			;;
		*macosx*)
			HOST="x86_64-apple-darwin"
			;;
	esac
}


# detect the tool type/name for the given file
detect_tooltype()
{
	case $1 in
		*ch55xduino-tools* )
			NAME=MCS51Tools
			VERSIONSTRING=$VERSION
			;;
		*sdcc* )
			NAME=sdcc
			VERSIONSTRING=build.$VERSION
			;;
	esac
}



### print a tool entry for the given file ############################



detect_tooltype "$TRUNK"
cat << EOF
                {
                    "name": "$NAME",
                    "version": "$VERSIONSTRING",
                    "systems": [
                        {
EOF
n=0
for FILE in $TRUNK*$VERSION.*; do
	if [ $n -gt 0 ]; then
	echo "                        },{"
	fi
	detect_hosttype "$FILE"
	echo "                            \"host\": \"$HOST\","
	print_filedata "$FILE"
	n=$((n+1))
done
cat << EOF
                        }
                    ]
                },
EOF
