'''
Created on 24/02/2012

@author: javierb
'''
from pymongo import Connection
from xml.dom.minidom import parse
import xpath

class Preprocessing(object):
    '''
    classdocs
    '''
    def __init__(self):
        self.db = Connection("psmongo1", 27017).PSDemo
        self.xmlTemplate = parse("preprocessing/preprocessing.xml")
        
    def __setFilters(self, filterName):
        namespace = "es.tid.ps.kpicalculation.cleaning." + filterName.replace(' ', '')
        path = "//property[name='kpifilters']/value"
        filters = xpath.find( path, self.xmlTemplate)
        if filters[0].firstChild.data == " ":
            filters[0].firstChild.data = namespace
        else:
            filters[0].firstChild.data += "," +namespace

    def __setFilterValues(self,propertyName,values):
        path = "//property[name='kpifilters." + propertyName.lower().replace(' ', '.') +"']/value"
        properties = xpath.find( path, self.xmlTemplate)
        for val in values:
            if properties[0].firstChild.data == " ":
                properties[0].firstChild.data = val
            else:
                properties[0].firstChild.data += "," +val
        
    def getOoziesXml(self, template):
        values = template['attribute_values']
        attributes = self.db.wizard_FixedTemplates.find( {"name" : "PreprocessingTemplate"} )[0]['attributes']
        for att in attributes:
            header = att['label']['header']
            for val in values:
                r = val['attribute']
                if header in r.keys() and r[header]['Enabled'] == "1":
                    self.__setFilters( header )
                    self.__setFilterValues( header, r[header]['Values'] )
        return self.xmlTemplate.toxml(None)



