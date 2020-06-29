#!/usr/bin/python
#-*- encoding=utf-8 -*-

import json,os,sys

host = "X.com"
url = "/x/y"

curl_cmd = "curl -s "
http_prefix = "http://"

def get(params):
    param_str = "?"
    for key in params:
        param_str += key
        param_str += "="
        param_str += params[key]
        param_str += "&"

    param_str = param_str[0:len(param_str) - 1]

    http_request = curl_cmd + http_prefix + host + url + param_str

    print "http request: " + http_request

    res = os.popen(http_request, 'r', 4096)

    return res.read()

def parse(json_str):
    json_obj = json.loads(json_str)
    if json_obj.has_key("results"):
        for result in json_obj["results"]:
            if result.has_key("nics"):
                for nic in result["nics"]:
                    if nic.has_key("ipv4"):
                        print nic["ipv4"]


def main(argv):
    if len(argv) == 0:
        print "usage: python ./xxx.py service_tree_path!"
        exit(-1)

    params = {"path":argv[0], "type":"json"}

    parse(get(params))

if __name__ == "__main__":
    main(sys.argv[1:])
