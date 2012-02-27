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
        self.xmlTemplate = parse("webprofiling/webprofiling.xml")
        
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
        properties[0].setAttribute("name", str(name))
        
        
    def getOoziesXml(self, template):
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
