#!/bin/bash

echo 'SOURCES += \'
find src -iname '*.cpp' | sed -e 's/.*/    & \\/'
echo

echo 'HEADERS += \'
find src -iname '*.hpp' | sed -e 's/.*/    & \\/'
