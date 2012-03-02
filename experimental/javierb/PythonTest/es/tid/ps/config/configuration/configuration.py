'''
Created on 27/02/2012

@author: javierb
'''
from pymongo import Connection
from pymongo.objectid import ObjectId
from xml.dom.minidom import parse
from xml.dom.minidom import Element,Node
from ..preprocessing.preprocessing import Preprocessing
from ..webprofiling.webprofiling import WebProfiling

import xpath,os

class Configuration(object):
    '''
    classdocs
    '''
    
    users = {"javierb" : 
            {"defaultPath":"/user/javierb/es/",
             "modules":
                {"4f436f74367cfd07b1000003":
                    {"landing":"landing",
                     "output":"output",
                     "fileNamePattern":"o2_gsma_gprs_hellcat30a_"
                    }
                }
            }
        }
    paths = {"configuration":"/user/javierb/system/config/"}
    
    def __init__(self, userId, moduleId):
        self.db = Connection("psmongo1", 27017).PSDemo
        self.xmlWorkflow = parse("templates/workflow.xml")
        self.xmlCoord = parse("templates/coordinator.xml")
        self.userId = userId
        self.moduleId = moduleId
        self.userconf = self.__class__.users[userId]
     
    def createConfiguration(self):
        templates = self.__getTemplatesIds() 

        for tempId in templates:
            temp = self.__getTemplateById(tempId)
            if temp['template'] == "IngestionTemplate":
                print("Ingestion Template")
            elif temp['template'] == "PreProcessingTemplate":
                p = Preprocessing()
                b = p.getOoziesXml(temp, self.userconf, self.moduleId)
                fileName = "PP_" + tempId + ".xml"
                f = open("./" + fileName ,"w")
                f.write(b)
                f.close()
                self.__addFilteringNode(fileName)
            else:
                wp = WebProfiling()
                wb = wp.getOoziesXml(temp, self.userconf, self.moduleId)  
                fileName = "WP_" + tempId + ".xml"
                f2 = open("./" +  fileName,"w")
                f2.write(wb)
                f2.close()
                self.__addWebProfilingNode(tempId, fileName)
    
        self.setCoordination()
    
        self.save()   
        
    def __getTemplatesIds(self):
        values = self.db.wizard_configuration.find({"_id":ObjectId(self.moduleId)})[0]['templates']
        return values
    
    def __getTemplateById(self, templateId):
        template = self.db.wizard_template.find({"_id":ObjectId(templateId)})[0]
        return template
    
    def __addFilteringNode(self, filePath):
        child = self.xmlWorkflow.createElement("xi:include")
        child.setAttribute('href', filePath)
        refPath = "//kill[@name='fail']"
        refNode = xpath.findnode(refPath, self.xmlWorkflow)
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        
    def __addWebProfilingNode(self, name, filePath):
        child = self.xmlWorkflow.createElement("xi:include")
        child.setAttribute('href', filePath)
        refPath = "//kill[@name='fail']"
        refNode = xpath.findnode(refPath, self.xmlWorkflow)
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        
        '''child = self.xmlWorkflow.createElement("path")
        child.setAttribute('start', 'action_' + name)
        path = "//fork[@name='aggregation']"
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        aggFork = xpath.findnode(path, self.xmlWorkflow)
        aggFork.nodeType = Node.ELEMENT_NODE
        aggFork.appendChild(child)'''
        
    def setCoordination(self):
        nameNode = "hdfs://pshdp01:8011"
        userHome = self.__class__.users[self.userId]['defaultPath'] 
        moduleFolder = self.moduleId
        landingArea = self.userconf['modules'][self.moduleId]['landing'] + "/"
        fileNameTemplate = self.userconf['modules'][self.moduleId]['fileNamePattern']
        timeSpec = "${YEAR}${MONTH}${DAY}"
        extension = ".dat.lzo"
        frequency = 1
        start= "2012-03-01T00:00Z" 
        end= "2012-03-01T00:00Z"
        inputStart= "2012-02-29T00:00Z"
        refPath = "//dataset[@name='din']/uri-template"
        refNode = xpath.findnode(refPath, self.xmlCoord)
        refNode.firstChild.data = nameNode + userHome + landingArea + fileNameTemplate + timeSpec + extension
        '''self.xmlCoord.firstChild.insertBefore(child, refNode)'''
        refPath = "//app-path"
        refNode = xpath.findnode(refPath, self.xmlCoord)
        refNode.firstChild.data = nameNode +  self.__class__.paths['configuration'] + self.userId + "/" + self.moduleId
        
    def save(self):
        fileName = "workflowout.xml"
        f2 = open(fileName,"w")
        f2.write(self.xmlWorkflow.toprettyxml())
        f2.close()
        fileName = "coordinator.xml"
        f2 = open(fileName, "w")
        f2.write(self.xmlCoord.toprettyxml())
        f2.close()
        os.system("xmllint --xinclude --format ./workflowout.xml > ./workflow.xml")
        os.system("hadoop dfs -rmr " + self.__class__.paths['configuration'] + self.userId + "/" + self.moduleId)
        os.system("hadoop dfs -mkdir " + self.userconf['defaultPath'] + self.userconf['modules'][self.moduleId]['landing'])
        os.system("hadoop dfs -put ./coordinator.xml " + self.__class__.paths['configuration'] + self.userId + "/" + self.moduleId + "/coordinator.xml")
        os.system("hadoop dfs -put ./workflow.xml " + self.__class__.paths['configuration'] + self.userId + "/" + self.moduleId + "/workflow.xml")
        os.system("rm -f *.xml")
        os.system("oozie job -run -D oozie.coord.application.path=hdfs://pshdp01:8011" + self.__class__.paths['configuration'] + self.userId + "/" + self.moduleId)