#!/bin/bash

check_file_exist() {
	if [ -f ${1} ]
    then
    	echo "file exist: ${1}"
    	return 0
    else
    	echo "file not exist: ${1}"
    	return 1
    fi
}

get_latest_log() {
	new_latest_log_file_number=${latest_log_file_number}
	ret=0
	while true
	do
		let new_latest_log_file_number+=1
		new_latest_log_file="${boost_log_file_prefix}_${new_latest_log_file_number}.${boost_log_file_suffix}"

		check_file_exist ${new_latest_log_file}
		ret=$?
		echo "next log file: ${new_latest_log_file}, exist(0-yes/1-no): ${ret}"

		if [ ${ret} -ne 0 ]
		then
			let new_latest_log_file_number-=1
			#new_latest_log_file="${boost_log_file_prefix}_${new_latest_log_file_number}.${boost_log_file_suffix}"

			if [ ${latest_log_file_number} -lt ${new_latest_log_file_number} ]
			then
				latest_log_file_number=${new_latest_log_file_number}
				#latest_log_file=${new_latest_log_file}
				latest_log_file="${boost_log_file_prefix}_${latest_log_file_number}.${boost_log_file_suffix}"
				ret=0
			else
				ret=1
			fi

			echo "latest log file is ${latest_log_file}, start log file is ${log_file}"
			return ${ret}
		fi
	done
}

handle_quit_signal() {
    echo "received signal: SIGTERM or SIGINT"
    if [ ${current_tail_pid} -ne -1 ]
    then
    	echo "now kill current tail process, pid is: ${current_tail_pid}"
    	kill -9 ${current_tail_pid}
    fi
    exit 0
}

trap 'handle_quit_signal' SIGTERM
trap 'handle_quit_signal' SIGINT


#	$1	dynamic	if 1, parse and replay the log dynamically; if 0, statically
#	$2	boost log file prefix
#	$3	boost log file suffix
#	$4	boost log file start number
#	$5	filter
#	%6	separation
#	%7	target column number

main() {
	if [ $# -lt 7 ]
    then
		echo "variable number: $#"
        echo "usage: $0 [dynamic] [boost_log_file_prefix] [boost_log_file_suffix] [boost_log_file_start_number] \
[filter] [separation] [target_column_number]"
        echo "e.g. $0 1 test log 0 \\\"docID\\\".*insert \\\" 4"
		exit 1
    fi

    dynamic=$1
    boost_log_file_prefix=$2
    boost_log_file_suffix=$3
    boost_log_file_start_number=$4
    filter=$5
    separation=$6
    target_column_number=$7

    echo "
dynamic: ${dynamic}
boost_log_file_prefix: ${boost_log_file_prefix}
boost_log_file_suffix: ${boost_log_file_suffix}
boost_log_file_start_number: ${boost_log_file_start_number}
filter: ${filter}
separation: ${separation}
target_column_number: ${target_column_number}
"

	log_file="${boost_log_file_prefix}_${boost_log_file_start_number}.${boost_log_file_suffix}"
	latest_log_file_number=${boost_log_file_start_number}
    latest_log_file=${log_file}
    current_process_id=$$
    current_tail_pid=-1
    echo "current process id is: ${current_process_id}"

	check_file_exist ${log_file}
	if [ $? -ne 0 ]
	then
		exit 1
	fi


#	2019-11-12|00:56:17.679667[SPCL][../dao/mongoDAO.cpp:302]: {"docID":"0Njb5ceA","operation":"unchanged",
#	"success":true,"timestamp":{"$date":1573520177000},"DBName":"staticFeature","collName":"document"}


	if [ ${dynamic} -eq 0 ]
	then
    	cat ${log_file} | awk -F${separation} -v filter=${filter} -v column=${target_column_number} '{
    		if($0~/'$filter'/) {
    			print $column;
    			system("curl -X GET http://10.136.132.13:15060/checkCorrectness?docID=" $column)
    		}
    	}'
    else
    	while true
    	do
    		get_latest_log

    		nohup tail -f ${latest_log_file} |
    			awk -F${separation} -v filter=${filter} -v column=${target_column_number} '{
					if($0~/'$filter'/) {
						print $column;
						system("curl -X GET http://10.136.132.13:15060/checkCorrectness?docID=" $column)
					}
				}' & #echo $! > tailLog.pid

			#current_tail_pid=`cat tailLog.pid`
			#let current_tail_pid-=1

			#current_tail_pid=$!
			#let current_tail_pid-=1

			current_tail_pid=`ps -ef | grep "tail -f ${latest_log_file}" |
				awk -v ppid=${current_process_id} '{if($3==ppid) print $2}'`
    		echo "current tail pid is: ${current_tail_pid}"

			count=0
    		while true
    		do
    			sleep 10

    			let count+=1
    			echo "detect count is: ${count}"

    			# 36*3600=129600
    			if [ ${count} -gt 12960 ]
    			then
    				echo "there is no new log file after 36h, server may be stopped, log replay exit"
    				exit 0
    			fi

    			get_latest_log
    			if [ $? -eq 0 ]
    			then
    				count=0
    				echo "now kill current tail process, pid is: ${current_tail_pid}"
    				kill -9 ${current_tail_pid}
    				break
    			fi
    		done
    	done
	fi
}

main $1 $2 $3 $4 $5 $6 $7