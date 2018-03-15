#!/bin/bash
for i in {1..4}; do 
    echo ITERATION $i;
    echo "BEGIN examples-initial/typecheck"
        echo $(basename "../examples/jpbasic_chkt_0$i.asl")
        ./asl ../examples/jpbasic_chkt_0$i.asl | egrep ^L > tmp.err
        diff tmp.err "${f/asl/err}"
        rm -f tmp.err
    echo "END   examples-initial/typecheck"


    echo ""
    echo "BEGIN examples-initial/codegen"
        echo $(basename ../examples/jpbasic_genc_0$i.asl)
        ./asl ../examples/jpbasic_genc_0$i.asl | egrep -v '^\(' > tmp.t
        diff tmp.t ${f/asl/t}
        rm -f tmp.t
    echo "END   examples-initial/codegen"
    echo ""
    echo ""
    echo "----------------------------"
    echo ""
    echo ""
done