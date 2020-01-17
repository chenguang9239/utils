#!/bin/sh

#hosts=( x.x.x.x y.y.y.y )

run() {
	rm -f "$0.log"
	i=0
	echo "num	IP		CPU			FQ		MEM		net_card		net_speed" | tee -a "$0.log"

	for ip in ${hosts[@]}
		do
		let i++
		ret=$(sshpass -p $password ssh -o StrictHostKeyChecking=no $user@$ip "cat /proc/cpuinfo | grep processor | wc -l;
		cat /proc/cpuinfo | grep \"cpu MHz\" | head -n 1 | awk -v column=4 '{print \$column}';
		free -h | sed -n '2p' | awk -v column=2 '{print \$column}';
		/sbin/ifconfig | grep \"eth[0-9]*:\" | awk -F':' -v column=1 '{print \$column}';
		/sbin/ifconfig | grep \"eth[0-9]*:\" | awk -F':' -v column=1 '{print \$column}' | xargs /sbin/ethtool 2>/dev/null | grep Speed | awk -v column=2 '{print \$column}'")

		splitRet=(${ret// / })
		logicCoreNum=${splitRet[0]}
		frequency=${splitRet[1]}
		memoryCapacity=${splitRet[2]}
		netCard=${splitRet[3]}
		netCardSpeed=${splitRet[4]}

		echo "${i}	${ip}		${logicCoreNum}		${frequency}		${memoryCapacity}		${netCard}		${netCardSpeed}" | tee -a "$0.log"
	done
}

parseHostsFromJSON() {
	splitValue=(${JSONValue//${key}/ })
	i=-1
	for s in ${splitValue[@]}
	do
		if [ $i -gt -1 ]
		then
			hosts[$i]=`echo $s | awk -F'"' '{print $3}'`
			echo "$i  ${hosts[$i]}"
		fi
		let i+=1
	done
}

parseHostsFromInput() {
	hosts=(${IPListValue//${key}/ })
}


#	$1	IP list type : IPList/IPListURL
#	$2	IPList value / IPListURL value
#	$3	key of IP in JSON string
# e.g.
# 	IPListURL
# 	http://palapala......&format=json
# 	test_key
#	name  passwd
# return string like:
# 	{......"test_key":"x.x.x.x"......}


main() {
	if [ $# -lt 5 ]
    then
		echo "variable number: $#"
        echo "usage: $0 [IPList/IPListURL] [IPList value / IPListURL value] [seperator / key of IP in JSON string] ssh_user ssh_password"
		exit 1
    fi

    type=$1
    key=$3
    user=$4
	password=$5

    if [ "${type}" == "IPListURL" ]
    then
    	IPListURLValue=$2
    	JSONValue=`curl -s ${IPListURLValue}`
		parseHostsFromJSON
    elif [ "${type}" == "IPList" ]
    then
    	IPListValue=$2
    	parseHostsFromInput
    else
    	echo "invalid type: ${type}"
        echo "usage: $0 [IPList/IPListURL] [IPList value / IPListURL value] [seperator / key of IP in JSON string] ssh_user ssh_password"
		exit 1
    fi

	run
}

main $1 $2 $3 $4 $5



#ret=`sshpass -p test ssh -o StrictHostKeyChecking=no test@x.x.x.x "cat /proc/cpuinfo | grep processor | wc -l;
#cat /proc/cpuinfo | grep \"cpu MHz\" | head -n 1 | awk -v column=4 '{print \$column}';
#free -h | sed -n '2p' | awk -v column=2 '{print \$column}';
#/sbin/ifconfig | grep \"eth[0-9]*:\" | awk -F':' -v column=1 '{print \$column}';
#/sbin/ifconfig | grep \"eth[0-9]*:\" | awk -F':' -v column=1 '{print \$column}' | xargs ethtool 2>/dev/null | grep Speed | awk -v column=2 '{print \$column}'"`
#
#sshpass -p test ssh -o StrictHostKeyChecking=no test@x.x.x.x "cat /proc/cpuinfo | grep processor | wc -l"
#sshpass -p test ssh -o StrictHostKeyChecking=no test@x.x.x.x "cat /proc/cpuinfo | grep \"cpu MHz\" | head -n 1 | awk -v column=4 '{print \$column}'"
#sshpass -p test ssh -o StrictHostKeyChecking=no test@x.x.x.x "free -h | sed -n '2p' | awk -v column=2 '{print \$column}'"
#sshpass -p test ssh -o StrictHostKeyChecking=no test@x.x.x.x "/sbin/ifconfig | grep \"eth[0-9]*:\" | awk -F':' -v column=1 '{print \$column}';"
#sshpass -p test ssh -o StrictHostKeyChecking=no test@x.x.x.x "/sbin/ifconfig | grep \"eth[0-9]*:\" | awk -F':' -v column=1 '{print \$column}' | xargs ethtool 2>/dev/null | grep Speed | awk -v column=2 '{print \$column}'"



