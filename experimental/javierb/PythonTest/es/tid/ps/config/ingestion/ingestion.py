'''
Created on 22/02/2012

@author: javierb
'''
from pymongo import Connection
from pymongo.objectid import ObjectId
from xml.dom.minidom import parse
import xpath

    
def setFilters(xml,filter):
    namespace = "es.tid.ps.kpicalculation.cleaning." + filter.replace(' ', '')
    path = "//property[name='kpifilters']/value"
    filters = xpath.find( path, xml)
    if filters[0].firstChild.data == " ":
        filters[0].firstChild.data = namespace
    else:
        filters[0].firstChild.data += "," +namespace

def setFilterValues(xml,property,values):
    path = "//property[name='kpifilters." + property.lower().replace(' ', '.') +"']/value"
    properties = xpath.find( path, xml)
    for val in values:
        if properties[0].firstChild.data == " ":
            properties[0].firstChild.data = val
        else:
            properties[0].firstChild.data += "," +val

connection = Connection("psmongo1", 27017)
db = connection.PSDemo
values = db.wizard_template.find({"_id":ObjectId("4f43705c367cfd07b1000005")})[0]['attribute_values']
attributes = db.wizard_FixedTemplates.find( {"name" : "PreprocessingTemplate"} )[0]['attributes']

doc = parse("filtering.xml")

for att in attributes:
    header = att['label']['header']
    for val in values:
        r = val['attribute']
        if header in r.keys() and r[header]['Enabled'] == "1":
            setFilters( doc, header )
            setFilterValues(doc, header,r[header]['Values'])
               

f = open("./output.xml",  "w")

f.write(doc.toxml(None))
