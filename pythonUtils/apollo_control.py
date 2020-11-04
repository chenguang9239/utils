#!/usr/bin/python
# -*- coding: UTF-8 -*-

import httplib
import urllib
import sys
import os
import getopt
import time
import json
import datetime
import collections

PORTAL_ADDRESS="XXXXXX:8107"
USERNAME="XXXXXX"
TOKEN="XXXXXX"
APPID="XXXXXX"

# 将在Apollo上修改的参数发布
def release(env, namespace):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 3:
                body = '{"releaseTitle":"auto release","releaseComment":"auto release","releasedBy":"' + USERNAME + '"}'
                url = '/openapi/v1/envs/' + env + '/apps/' + APPID + '/clusters/default/namespaces/' + namespace + '/releases'
                conn.request('POST', url, body, headers)
                response = conn.getresponse()
                data = response.read()
                print "response: " + data
            break
        except:
            restart = restart + 1
            conn.close()

def addValue(env, namespace, key, value, comment):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 3:
                body = '{"key":"' + key + '","value":"' + generatePlainStr(value) + '","comment":"' + comment + '","dataChangeCreatedBy":"' + USERNAME + '"}'
                url = '/openapi/v1/envs/' + env + '/apps/' + APPID + '/clusters/default/namespaces/' + namespace + '/items/'
                conn.request('POST', url, body, headers)
                response = conn.getresponse()
                data = response.read()
                print "response: " + data
                break
        except:
            restart = restart + 1
            conn.close()

def isValidJSON(value):
    try:
        json.loads(value)
        print "valid JSON: " + value
        return True
    except ValueError:
        print "invalid JSON: " + value
        return False
    except:
        print "other exception, value: " + value
        return False

def generatePlainStr(value):
    if type(value) == str:
        if isValidJSON(value):
            return value.replace('"','\\"')
        else:
            return value
    return value

# 将env环境下的namespace中的key值修改为value
# value needs to be string type, not a JSON block. see https://github.com/ctripcorp/apollo/wiki/Apollo%E5%BC%80%E6%94%BE%E5%B9%B3%E5%8F%B0
# so the items in the body need to like: string key - string value,
# so the " in value need to be escaped
def modify(env, namespace, key, value):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 3:
                body = '{"key":"' + key + '","value":"' + generatePlainStr(value) + '","dataChangeLastModifiedBy":"' + USERNAME + '"}'
                url = '/openapi/v1/envs/' + env + '/apps/' + APPID + '/clusters/default/namespaces/' + namespace + '/items/' + key
                print "debug body: " + body
                conn.request('PUT', url, body, headers)
                response = conn.getresponse()
                data = response.read()
                print "response: " + data
            break
        except:
            restart = restart + 1
            conn.close()

def getValue(env, namespace, key):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 3:
                url = '/openapi/v1/envs/' + env + '/apps/' + APPID + '/clusters/default/namespaces/' + namespace
                conn.request('GET', url, '', headers)
                response = conn.getresponse()
                data = response.read()
                configs = json.loads(data, object_pairs_hook=collections.OrderedDict)
                value = ''
                for config in configs['items']:
                    if config['key'] == key:
                        value = config['value']
                        break
                return value
            break
        except:
            restart = restart + 1
            conn.close()

def parseJSONValue(value, jsonKey):
    jsonStruct = json.loads(value, object_pairs_hook=collections.OrderedDict)
    jsonValue = ''
    if jsonStruct.has_key(jsonKey):
        jsonValue = jsonStruct[jsonKey]
        print "jsonKey: " + jsonKey + ", jsonValue: " + jsonValue
    else:
        print "not exists jsonKey: " + jsonKey  + ", value: " + value
    return jsonValue

def insertJSONValue(value, jsonKey, jsonValue):
    newJSONValue = ''
    jsonStruct = json.loads(value, object_pairs_hook=collections.OrderedDict)
    if not jsonStruct.has_key(jsonKey):
        jsonStruct[jsonKey] = jsonValue
        newJSONValue = json.dumps(jsonStruct, sort_keys=False)
        newJSONValue = generatePlainStr(newJSONValue)
        print "new jsonValue: " + newJSONValue
    else:
        print "jsonKey exists and skip insert, value: " + value
    return newJSONValue

def modifyJSONValue(value, jsonKey, jsonValue):
    newJSONValue = ''
    jsonStruct = json.loads(value, object_pairs_hook=collections.OrderedDict)
    if jsonStruct.has_key(jsonKey):
        jsonStruct[jsonKey] = jsonValue
        newJSONValue = json.dumps(jsonStruct, sort_keys=False)
        newJSONValue = generatePlainStr(newJSONValue)
        print "new jsonValue: " + newJSONValue
    else:
        print "jsonKey not exists and skip modify, value: " + value
    return newJSONValue

def getJSONValue(env, namespace, key, jsonKey):
    jsonValue = ''
    value = getValue(env, namespace, key)
    print "env: " + env + ", namespace: " + namespace + ", key: " + key + ", value: " + value
    if value != '':
        jsonValue = parseJSONValue(value, jsonKey)
        print "jsonKey: " + jsonKey + ", jsonValue: " + jsonValue
    return jsonValue

def addJSONValue(env, namespace, key, comment, jsonKey, jsonValue):
    value = getValue(env, namespace, key)
    newValue = insertJSONValue(value, jsonKey, jsonValue)
    if newValue != '':
        addValue(env, namespace, key, newValue, comment)
    else:
        print "new value empty, skip add JSON value"

def modifyJSON(env, namespace, key, jsonKey, jsonValue):
    value = getValue(env, namespace, key)
    newValue = modifyJSONValue(value, jsonKey, jsonValue)
    if newValue != '':
        modify(env, namespace, key, newValue)
    else:
        print "new value empty, skip modify JSON"

def createNamespace(namespace):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 3:
                body = '{"name":"'+namespace+'","appId":"'+APPID+'","format":"properties","isPublic":"false","Comment":"new type","dataChangeCreatedBy":"'+USERNAME+'"}'
                url = '/openapi/v1/apps/' + APPID + '/appnamespaces'
                conn.request('POST', url, body, headers)
                response = conn.getresponse()
                data = response.read()
                print "response: " + data
            break
        except:
            restart = restart + 1
            conn.close()

    for env in ENVS:
        addValue(env, namespace, 'isDeleted', '0', 'if namespace abandoned')

def getAllConfigs(env, namespace):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 5:
                time.sleep(1)
                url = '/openapi/v1/envs/' + env + '/apps/' + APPID + '/clusters/default/namespaces/' + namespace + '/releases/latest'
                conn.request('GET', url, '', headers)
                response = conn.getresponse()
                data = response.read()
                configs = json.loads(data, object_pairs_hook=collections.OrderedDict)
                return configs['configurations']
            break
        except:
            restart = restart + 1
            conn.close()

def getNamespaceConfig(env, namespace):
    headers = {"Content-type": "application/json;charset=UTF-8", "Authorization": TOKEN}

    restart = 0
    while True:
        conn = httplib.HTTPConnection(PORTAL_ADDRESS)
        try:
            if restart < 3:
                url = '/openapi/v1/envs/' + env + '/apps/' + APPID + '/clusters/default/namespaces/' + namespace
                conn.request('GET', url, '', headers)
                response = conn.getresponse()
                data = response.read()
                return data
            break
        except:
            restart = restart + 1
            conn.close()

def usage():
    # print 'apollo_control.py -n namespace -k value -v value '
    print 'please check your code'

def main(argv):
    # try:
    #     opts, args = getopt.getopt(argv, "n:k:v:")
    # except getopt.GetoptError:
    #     usage()
    #     sys.exit(1)

    # for opt, arg in opts:
    #     if opt == '-n':
    #         namespase = arg
    #     if opt == '-k':
    #         key = arg
    #     if opt == '-v':
    #         value = arg

    key_condition = 'XXXXXX'
    value_condition = 'YYYYYY'
    key_target = 'AAAAAA'
    value_target = 1

    env = 'PRO'
    namespase = 'NNNNNN'
    nmConfigs = getNamespaceConfig(env, namespase)

    print "debug: " + nmConfigs

    if 'namespace not found' not in nmConfigs:
        if type(nmConfigs) != str:
            print "skip nmConfigs: " + nmConfigs
            return

        totalStruct = json.loads(nmConfigs, object_pairs_hook=collections.OrderedDict)
        if totalStruct.has_key('items'):
            configList = totalStruct['items']
            for configStruct in configList:
                key = configStruct['key']
                value = configStruct['value']

                try:
                    valueStruct = json.loads(value, object_pairs_hook=collections.OrderedDict)
                    if valueStruct.has_key(key_condition):
                        if valueStruct[key_condition].startswith(value_condition):
                            if valueStruct.has_key(key_target):
                                print "begin to modify key: " + key + ", old value: " + value
                                modifyJSON(env, namespase, key, key_target, value_target)
                            else:
                                print "begin to insert key: " + key + ", old value: " + value
                                addJSONValue(env, namespase, key, key_target, value_target)
                        else:
                            print "skip key: " + key + ", value: " + value
                except ValueError:
                    print "exception, skip key: " + key + ", value: " + value
                    continue
        else:
            print "no items, totalStruct: " + totalStruct
    # release(env, namespace)

if __name__ == "__main__":
    main(sys.argv[1:])











