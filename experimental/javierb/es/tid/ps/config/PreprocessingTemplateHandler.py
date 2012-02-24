'''
Created on 23/02/2012

@author: javierb
'''
import xml.sax.handler

class PreprocessingTemplateHandler(xml.sax.handler.ContentHandler):
    '''
    classdocs
    '''

    def __init__(self):
        self.jobTracker = "${jobTracker}"
        self.nameNode = "${nameNode}"
        self.configuration = "configuration"
        

