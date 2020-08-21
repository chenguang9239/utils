#!/usr/bin/python
#-*- encoding=utf-8 -*-

from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
from urlparse import urlparse, parse_qs
import json

mock_data1 = 'mock_data1'
mock_data2 = 'mock_data2'

def do_mock1(self):
    global mock_data1
    print 'mock_data1: ' + mock_data1
    response = {
        'data': mock_data1,
        'header': {
            'code': 0,
            'msg': '操作成功'
        }
    }
    self._set_headers()
    self.wfile.write(json.dumps(response))

def do_mock2(self):
    response = {
        'data': mock_data2,
        'header': {
            'code': 0,
            'msg': '操作成功'
        }
    }
    self._set_headers()
    self.wfile.write(json.dumps(response))

def get_param(self, key):
    query_components = parse_qs(urlparse(self.path).query)
    print query_components
    return query_components[key][0]

def do_set_mock1(self, user_mock_data):
    global mock_data1
    mock_data1 = user_mock_data
    print 'mock_data1: ' + mock_data1
    response = {
        'mock_data1': mock_data1
    }
    self._set_headers()
    self.wfile.write(json.dumps(response))

class RequestHandler(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def do_GET(self):
	print 'path:' + self.path

        if self.path == '/mock1':
            do_mock1(self)
        elif self.path == '/mock2':
            do_mock2(self)
        elif self.path.startswith('/set_mock1'):
            do_set_mock1(self, get_param(self, 'data'))

    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length)
        print 'post data from client:'
        print post_data

        response = {
            'status':'SUCCESS',
            'data':'server got your post data'
        }
        self._set_headers()
        self.wfile.write(json.dumps(response))

def run():
    port = 5657
    print('Listening on localhost:%s' % port)
    server = HTTPServer(('10.136.127.23', port), RequestHandler)
    server.serve_forever()

run()
