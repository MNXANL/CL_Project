FILE1="$1"


./asl ../examples/jp_genc_$FILE1.asl | egrep -v '^\(' > tmp.t
echo "Compilation done"
#diff tmp.t ../examples/jp_genc_$(1).t
#ejecutar t con input in
../tvm/tvm tmp.t <../examples/jp_genc_$FILE1.in >tmp.out
diff -s tmp.out ../examples/jp_genc_$FILE1.out
