bash Syntaxer.sh
for i in {1..$1}; do
	grun Asl program ../examples/jp_chkt_0$i.asl 
done
