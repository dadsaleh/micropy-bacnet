#!/bin/sh
cd "$(dirname "$0")" || exit 1
mkdir -p build

status=0
for t in test/test_*.c; do
    bin="build/$(basename "$t" .c)"
    gcc -std=c23 -Wall -Wextra -Ipdu_decoder -Ipdu_decoder/core -Ivendor/unity \
        "$t" $(find pdu_decoder -name '*.c') vendor/unity/unity.c -o "$bin" && "./$bin" || status=1
done
exit $status
