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

' configId = 4f56289a218fdc2454000003'
' configId = 4f562a82218fdc26e8000004'
configureBdpTask('javierb', sys.argv[1])