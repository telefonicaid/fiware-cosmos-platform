#!/usr/bin/env python
# -*- coding: utf-8 -*-

import pycurl, json, StringIO, re, sys

STREAM_URL = "https://stream.twitter.com/1/statuses/sample.json"
REST_URL = "http://api.twitter.com/1/"



USER = "jgestwit"
PASS = "tururu01"

class Client:
  def __init__(self):
    self.buffer = ""
    self.conn = pycurl.Curl()
    self.conn.setopt(pycurl.USERPWD, "%s:%s" % (USER, PASS))
    self.conn.setopt(pycurl.URL, STREAM_URL)
    self.conn.setopt(pycurl.WRITEFUNCTION, self.on_receive)
    self.conn.perform()

  def on_receive(self, data):
    self.buffer += data
    if data.endswith("\r\n") and self.buffer.strip():
      content = json.loads(self.buffer)
      self.buffer = ""
      #print content
      if "text" in content:
        cleanText = re.sub(r'\n',r' ',content['text'])
	#print cleanText
	outString = "user=@" + content['user']['screen_name'] + " lang=" + content['user']['lang'] + ": " + cleanText
	print outString.encode('utf-8')
        #print u"user=@{0[user][screen_name]} lang={0[user][lang]}: {1}".format(content, cleanText)
        #print u"user=@{0[user][screen_name]} {1}".format(content, cleanText)

client = Client()
