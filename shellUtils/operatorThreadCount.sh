proc=(`ps aux | grep knn_  | grep threadQty | awk '{print $2 }'`)

cnt=0
for p in ${proc[@]}
do
echo "ps -mq $p | wc -l"
i=`ps -mq $p | wc -l`
#i=`ps -Lf $p | wc -l`
echo "$i"
let cnt+=i
        done
echo "$cnt"
