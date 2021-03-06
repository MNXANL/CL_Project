#!/bin/bash
for i in {1..4}; do 
    echo ITERATION $i;
    echo "BEGIN examples-initial/typecheck"
        echo $(basename "../examples/jpbasic_chkt_0$i.asl")
        ./asl ../examples/jpbasic_chkt_0$i.asl | egrep ^L > tmp.err
        diff -s tmp.err ../examples/jpbasic_chkt_0$i.err
        rm -f tmp.err
    echo "END   examples-initial/typecheck"


    #echo ""
    #echo "BEGIN examples-initial/codegen"
    #    echo $(basename ../examples/jpbasic_genc_0$i.asl)
    #    ./asl ../examples/jpbasic_genc_0$i.asl | egrep -v '^\(' > tmp.t
    #    diff -s tmp.t ../examples/jpbasic_genc_0$i.t
    #    rm -f tmp.t
    #echo "END   examples-initial/codegen"
    #echo ""
    #echo ""
    #echo "----------------------------"
    #echo ""
    #echo ""
done

for i in {1..4}; do 
    echo ITERATION $i;
    echo "BEGIN examples-initial/typecheck"
        echo $(basename "../examples/jp_chkt_0$i.asl")
        ./asl ../examples/jp_chkt_0$i.asl | egrep ^L > tmp.err
        diff -s tmp.err ../examples/jp_chkt_0$i.err
        rm -f tmp.err
    echo "END   examples-initial/typecheck"


    #echo ""
    #echo "BEGIN examples-initial/codegen (WILL NOT PASS)"
    #    echo $(basename ../examples/jp_genc_0$i.asl)
   #     ./asl ../examples/jp_genc_0$i.asl | egrep -v '^\(' > tmp.t
   #     diff -s tmp.t ../examples/jp_genc_0$i.t
   #     rm -f tmp.t
   # echo "END   examples-initial/codegen"
   # echo ""
   # echo ""
    #echo "----------------------------"
    #echo ""
    #echo ""
done
