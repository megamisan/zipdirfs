#!/usr/bin/env bash

set -e
set -u

basedir="$(dirname "$(readlink -f "$(dirname $0)")")/"
scriptmtime="$(stat -c %Y $0)"
testmtime="$(stat -c %Y $2/$1)"
exec 3>&1
exec 1>coveragec.log

makeTestList() {
	local base_name=""
	while IFS= read -u 4 line; do
		if echo ${line} | grep -E '^[A-Za-z_]+\.$' > /dev/null; then
			echo "${line}"'*'
		fi;
	done 4< <("$1" --gtest_list_tests)
}
testList="$(makeTestList "$2/$1")"
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
outputdir="${basedir}coverage/$1-outc/"

mkdir -p "${coveragedir}tmp/" "${outputdir}"
find "$2" -name \*.gcov -print0 | xargs -0r -- rm

TOTALCMD=()
updated=0
totalmtime=0
if [ ! -f "${coveragedir}totalc.info" ]; then updated=1
else totalmtime=$(stat -c %Y "${coveragedir}totalc.info"); fi

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
index=2
for test in ${testList}; do
	if isOld "${coveragedir}${test/.*/}.info"; then
		find "$2" -name \*.gcda -print0 | xargs -0r -- rm
		"$2/$1" --gtest_filter="${test}" > /dev/null
		echo Generating "${coveragedir}${test/.*/}.info" >&3
		lcov --config-file="$2/lcovrc" --no-external --capture --test-name "${test/.*/}" \
			--directory "$2" --output-file "${coveragedir}tmp/${test/.*/}.info"
		cleanTrace "$2" "${coveragedir}tmp/${test/.*/}.info" "${coveragedir}${test/.*/}.info"
	fi
	TOTALCMD[${index}]="--add-tracefile"
	TOTALCMD[$((index + 1))]="${coveragedir}${test/.*/}.info"
	index=$((index + 2))
	if [ ${updated} == 0 ] && \
	[ "$(stat -c %Y "${coveragedir}${test/.*/}.info")" -gt "${totalmtime}" ]; then updated=1; fi
done

if [ "${updated}" == "1" ] || [ "${scriptmtime}" -gt "${totalmtime}" ]; then
	echo Generating "${coveragedir}totalc.info" >&3
	lcov --config-file="$2/lcovrc" "${TOTALCMD[@]}" \
		--output-file "${coveragedir}totalc.info"
fi

if isOld "${outputdir}index.html" || \
	[ "$(stat -c %Y "${coveragedir}totalc.info")" -gt \
	"$(stat -c %Y "${outputdir}index.html")" ]; then
	find "${outputdir}" -type f -print0 | xargs -0r -- rm
	echo Generating "${outputdir}" >&3
	genhtml --config-file="$2/lcovrc" --prefix "$2" --ignore-errors source \
		"${coveragedir}totalc.info" --demangle-cpp --show-details \
		--legend --title "$1 case coverage" --output-directory="${outputdir}"
fi
