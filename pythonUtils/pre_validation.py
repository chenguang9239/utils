#!/usr/bin/python
#-*- encoding=utf-8 -*-

import io,os,sys,libconf,time

def curl(url):
    curl_cmd = "curl -w %{http_code} -s -o /dev/null "
    res = os.popen(curl_cmd + url,  'r', 512)
    return res.read()

def ping(addr):
    ping_cmd_start = "ping -c 1 "
    ping_cmd_end = " > /dev/null 2>&1"
    host = addr.split(':')
    return os.system(ping_cmd_start + host[0] + ping_cmd_end)

def main(argv):
    if len(argv) == 0:
        print "usage: python ./pre_validation.py config_file_path!"
        exit(-1)

    cfg_file_name = argv[0]
    print "config file is: " + cfg_file_name

    cfg_file = io.open(cfg_file_name, encoding='utf-8')

    cfg = libconf.load(cfg_file)

    if cfg.has_key("service"):
        if cfg["service"].has_key("xxx_addr"):
            addr = cfg["service"]["xxx_addr"]
            if ping(addr) != 0:
                print "ping failed, addr: " << addr
                exit(-1)

        if cfg["service"].has_key("xxx_url"):
            url = cfg["service"]["xxx_url"]
            res_code = curl(url)
            if res_code != '200':
                print "curl url failed: " + res_code + ", url: " + url
                exit(-1)


    time.sleep(0.1)




if __name__ == "__main__":
    main(sys.argv[1:])









