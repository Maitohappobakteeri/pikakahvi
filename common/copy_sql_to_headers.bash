#!/usr/bin/env bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
cd "$DIR"

rm -Rf generated/sql
mkdir -p generated/sql

# Generate headers
for FILE in $(find sql -name *.sql); do
    cd "$DIR"
    HEADERFILE="generated/$(echo $FILE | sed -r 's/(.*)\.sql/\1.h/g')"
    mkdir -p "$(dirname $HEADERFILE)"
    cd "$DIR/$(dirname $FILE)"
    xxd -i "$(basename $FILE)" > "$DIR/$HEADERFILE"
    echo "Copying $DIR/$HEADERFILE to $(basename $FILE) in $DIR/$(dirname $FILE)"
done
cd "$DIR"

# Generate upgrade list
UPGRADES=""
while read -r LINE; do
    # WARNING: ONLY 0-9 and a-z allowed
    LINE="$(echo $LINE | sed -r 's/[^0-9a-z ]/_/g')"
    LINE="$(echo "$LINE" | sed -r 's/(.*) (.*) (.*)/    \{ \1, \"\2\", std::string(reinterpret_cast<const char*>(\3)) \},/')"
    UPGRADES="$UPGRADES$LINE"$'\n'
done <<<$(./build/create-db a)

# Generate includes
ALLINCLUDES=""
for FILE in $(find sql -name *.sql); do
    HEADERFILE="generated/$(echo $FILE | sed -r 's/(.*)\.sql/\1.h/g')"
    ALLINCLUDES="$ALLINCLUDES#include \"$HEADERFILE\""$'\n'
done

echo "
#ifndef GENERATE_SQL_HEADER_HPP
#define GENERATE_SQL_HEADER_HPP


#include <string>
#include <vector>

$ALLINCLUDES

struct SqlUpgrade {
    int version; std::string type; std::string script;
};


std::vector<SqlUpgrade> sqlUpgrades = {
$UPGRADES
};


#endif
" > generated/generate_sql_header.hpp
