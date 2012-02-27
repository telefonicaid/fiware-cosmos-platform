'''
Created on 27/02/2012

@author: javierb
'''
from pymongo import Connection
from pymongo.objectid import ObjectId
from xml.dom.minidom import parse
from xml.dom.minidom import Element,Node

import xpath

class Configuration(object):
    '''
    classdocs
    '''
    def __init__(self):
        self.db = Connection("psmongo1", 27017).PSDemo
        self.xmlTemplate = parse("configuration/workflow.xml")
        
    def getTemplatesIds(self, valuesId):
        values = self.db.wizard_configuration.find({"_id":ObjectId(valuesId)})[0]['templates']
        return values
    
    def getTemplateById(self, templateId):
        template = self.db.wizard_template.find({"_id":ObjectId(templateId)})[0]
        return template
    
    def addFilteringNode(self, filePath):
        child = self.xmlTemplate.createElement("xi:include")
        child.setAttribute('href', filePath)
        refPath = "//fork[@name='aggregation']"
        refNode = xpath.findnode(refPath, self.xmlTemplate)
        self.xmlTemplate.firstChild.insertBefore(child, refNode)
        
    def addWebProfilingNode(self, name, filePath):
        child = self.xmlTemplate.createElement("xi:include")
        child.setAttribute('href', filePath)
        refPath = "//join[@name='joining']"
        refNode = xpath.findnode(refPath, self.xmlTemplate)
        self.xmlTemplate.firstChild.insertBefore(child, refNode)
        
        child = self.xmlTemplate.createElement("path")
        child.setAttribute('start', name)
        path = "//fork[@name='aggregation']"
        aggFork = xpath.findnode(path, self.xmlTemplate)
        aggFork.nodeType = Node.ELEMENT_NODE
        aggFork.appendChild(child)
        
    def save(self):
        fileName = "workflowout.xml"
        f2 = open(fileName,"w")
        f2.write(self.xmlTemplate.toprettyxml())