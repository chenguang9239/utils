#!/bin/bash

log_init() {
    type_log_path=$1
    log_name=$2
    export LOG_NAME="${type_log_path}/${log_name}.log"
    if [ ! -d ${type_log_path} ]; then
        mkdir -p ${type_log_path}
        log_warn "[log.sh][log_init] check no ${type_log_path} log dir, create a new one"
    fi
    log_info "[log.sh][log_init] log set complete. Log name : ${LOG_NAME}"
}

log_err() {
  #echo -e "\033[31m [$(date +'%Y-%m-%dT%H:%M:%S')] [ERROR] $@ \033[0m" >&1  | tee -a ${LOG_NAME}
  echo "[$(date +'%Y-%m-%dT%H:%M:%S')] [ERROR] $@" >&1  | tee -a ${LOG_NAME}
}

log_fatal() {
  echo "[$(date +'%Y-%m-%dT%H:%M:%S')] [FATAL] $@" >&1  | tee -a ${LOG_NAME}
  exit 1
}

log_warn() {
  #echo -e "\033[31m [$(date +'%Y-%m-%dT%H:%M:%S')] [WARN] $@ \033[0m" >&1  | tee -a ${LOG_NAME}
    echo "[$(date +'%Y-%m-%dT%H:%M:%S')] [WARN] $@" >&1  | tee -a ${LOG_NAME}
}

log_info() {
  echo "[$(date +'%Y-%m-%dT%H:%M:%S')] [INFO] $@" >&1  | tee -a ${LOG_NAME}
}

sleep_with_log() {
    sleep_count=$(( $1/10 ))
    for ((zzz=1; zzz<=sleep_count; zzz++))
    do
    	log_info $2" sleeping (10s count : $zzz), total $1"
	    sleep 10
    done
}

# in $1: ret $2: message
check_ret() {
    if [ $1 != 0 ]
    then
        log_err "${@:2}"
    fi
    log_info "${@:2}"
}

# example:

#	#!/bin/bash
#
#	source ./log.sh
#
#	log_init "./shell_log" "test"
#
#	log_info "test log info"
#
#	log_warn "test log warn"
#
#	log_err "test log err"
#
#	sleep_with_log 55 sleep55
#
#	log_fatal "test log fatal"