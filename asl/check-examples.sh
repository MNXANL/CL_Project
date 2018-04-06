#!/bin/bash

echo "BEGIN examples-initial/typecheck"
for f in ../examples/jpbasic_chkt_*.asl; do
    echo $(basename "$f")
    ./asl "$f" | egrep ^L > tmp.err
    diff tmp.err "${f/asl/err}"
    rm -f tmp.err
done
echo "END   examples-initial/typecheck"

echo "-------------------------------"
echo "BEGIN examples-full/typecheck"
for f in ../examples/jp_chkt_*.asl; do
    echo $(basename $f)
    ./asl $f | egrep ^L > tmp.err
    diff tmp.err ${f/asl/err}
    rm -f tmp.err
done
echo "END   examples-full/typecheck"

