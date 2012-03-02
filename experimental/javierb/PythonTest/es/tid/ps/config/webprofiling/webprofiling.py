'''
Created on 24/02/2012

@author: javierb
'''
from pymongo import Connection
from pymongo.objectid import ObjectId
from xml.dom.minidom import parse
import xpath

class WebProfiling(object):
    '''
    classdocs
    '''
    dict = {"Attributes Fields" : "fields", "Grouping Field": "group"}
    
    def __init__(self):
        self.db = Connection("psmongo1", 27017).PSDemo
        self.xmlTemplate = parse("templates/webprofiling.xml")
        
    def setValues(self,propertyName,values):
        path = "//property[name='kpi.aggregation." + propertyName +"']/value"
        properties = xpath.find( path, self.xmlTemplate)
        for val in values:
            if properties[0].firstChild.data == " ":
                properties[0].firstChild.data = val
            else:
                properties[0].firstChild.data += "," +val
    
    def setName(self, name):
        path = "//action"
        properties = xpath.find( path, self.xmlTemplate)
        properties[0].setAttribute("name", 'aggregation')
        '''properties[0].setAttribute("name", 'action_' + str(name))'''
    
    def __setPaths(self, template, moduleId, userConfig):
        path = "//property[name='mapred.input.dir']/value"
        inputProperty = xpath.find( path, self.xmlTemplate)
        inputProperty[0].firstChild.data = "${nameNode}" + userConfig['defaultPath'] + userConfig['modules'][moduleId]['output'] + "/cleaned/" + "${outputDir}" 
        
        path = "//property[name='mapred.output.dir']/value"
        inputProperty = xpath.find( path, self.xmlTemplate)
        inputProperty[0].firstChild.data = "${nameNode}" + userConfig['defaultPath'] + userConfig['modules'][moduleId]['output'] + "/kpis/" + str(template['_id']) + "/${outputDir}"
        
    def getOoziesXml(self, template, userConfig, moduleId):
        self.__setPaths(template, moduleId, userConfig)
        values = template['attribute_values']
        self.setName(template['_id'])
        attributes = self.db.wizard_FixedTemplates.find( {"name" : "WebProfilingTemplate"} )[0]['attributes']
        for att in attributes:
            header = att['label']['header']
            for val in values:
                r = val['attribute']
                if header in r.keys():
                    self.setValues( self.dict[header], r[header] )
        return self.xmlTemplate.toxml(None)
