#!/usr/bin/env bash

set -e
set -u

basedir="$(dirname "$(readlink -f "$(dirname $0)")")/"
scriptmtime="$(stat -c %Y $0)"
testmtime="$(stat -c %Y $2/$1)"
exec 3>&1
exec 1>coverage.log

cleanTrace() {
	lcov --config-file="$1/lcovrc" --remove "$2" \
		'/usr/include/*' '/usr/lib/*' \
		"$1"'/test/*' "$1"'/googletest/*' \
		-o "$3"
}
isOld() {
	if [ -f "$1" ]; then
		local mtime="$(stat -c %Y "$1")"
		[ "${scriptmtime}" -gt "${mtime}" ] || [ "${testmtime}" -gt "${mtime}" ]
	else
		return 0
	fi
}

coveragedir="${basedir}coverage/$1-files/"
outputdir="${basedir}coverage/$1-out/"

mkdir -p "${coveragedir}tmp/" "${outputdir}"
find "$2" -name \*.gcov -print0 | xargs -0r -- rm

TOTALCMD=()
updated=0
totalmtime=0
if [ ! -f "${coveragedir}total.info" ]; then updated=1
else totalmtime=$(stat -c %Y "${coveragedir}total.info"); fi

if isOld "${coveragedir}base.info"; then
	find "$2" -name \*.gcda -print0 | xargs -0r -- rm
	echo Generating "${coveragedir}base.info" >&3
	lcov --config-file="$2/lcovrc" --no-external --capture --initial \
		--directory "$2" --output-file "${coveragedir}tmp/base.info"
	cleanTrace "$2" "${coveragedir}tmp/base.info" "${coveragedir}base.info"
fi
TOTALCMD[0]="--add-tracefile"
TOTALCMD[1]="${coveragedir}base.info"
if [ ${updated} == 0 ] && \
[ "$(stat -c %Y "${coveragedir}base.info")" -gt "${totalmtime}" ]; then updated=1; fi
if isOld "${coveragedir}all.info"; then
	find "$2" -name \*.gcda -print0 | xargs -0r -- rm -v >&3
	"$2/$1" > /dev/null
	echo Generating "${coveragedir}all.info" >&3
	lcov --config-file="$2/lcovrc" --no-external --capture --test-name all \
		--directory "$2" --output-file "${coveragedir}tmp/all.info"
	cleanTrace "$2" "${coveragedir}tmp/all.info" "${coveragedir}all.info"
fi
TOTALCMD[2]="--add-tracefile"
TOTALCMD[3]="${coveragedir}all.info"
if [ ${updated} == 0 ] && \
[ "$(stat -c %Y "${coveragedir}all.info")" -gt "${totalmtime}" ]; then updated=1; fi

if [ "${updated}" == "1" ] || [ "${scriptmtime}" -gt "${totalmtime}" ]; then
	echo Generating "${coveragedir}total.info" >&3
	lcov --config-file="$2/lcovrc" "${TOTALCMD[@]}" \
		--output-file "${coveragedir}total.info"
fi

if isOld "${outputdir}index.html" || \
	[ "$(stat -c %Y "${coveragedir}total.info")" -gt \
	"$(stat -c %Y "${outputdir}index.html")" ]; then
	find "${outputdir}" -type f -print0 | xargs -0r -- rm
	echo Generating "${outputdir}" >&3
	genhtml --config-file="$2/lcovrc" --prefix "$2" --ignore-errors source \
		"${coveragedir}total.info" --demangle-cpp \
		--legend --title "$1 coverage" --output-directory="${outputdir}"
fi
