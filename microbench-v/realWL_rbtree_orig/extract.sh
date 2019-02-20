

root_list=`awk '{print $NF}' trace_orig | sort | uniq -c | sort -nr`

for x in $root_list; do
	fname="$x".trc
	echo $x
	awk 
>>
done 

