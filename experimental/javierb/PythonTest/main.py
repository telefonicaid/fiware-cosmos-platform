'''
Created on 22/02/2012

@author: javierb
'''
from es.tid.ps.config.configuration.configuration import Configuration
from es.tid.ps.config.preprocessing.preprocessing import Preprocessing
from es.tid.ps.config.webprofiling.webprofiling import WebProfiling
import sys

def configureBdpTask(userId, configurationId):
    config = Configuration(userId, configurationId)
    config.createConfiguration()  

' configId = 4f436f74367cfd07b1000003'
configureBdpTask('javierb', sys.argv[1])