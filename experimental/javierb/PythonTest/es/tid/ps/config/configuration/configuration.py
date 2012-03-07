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
                {"4f56289a218fdc2454000003":
                    {"landing":"landing",
                     "output":"output",
                     "fileNamePattern":"o2_gsma_gprs_hellcat30a_"
                    },
                "4f562a82218fdc26e8000004":
                    {"landing":"landing",
                     "output":"output2",
                     "fileNamePattern":"o2_gsma_gprs_hellcat30a_"
                    }
                }
                
            }
        }
    paths = {"configuration":"/user/javierb/system/config/"}
    
    def __init__(self, userId, moduleId):
        self.db = Connection("psmongo1", 27017).PSCONFIGJAVI2
        self.xmlWorkflow = parse("templates/workflow.xml")
        self.xmlCoord = parse("templates/coordinator.xml")
        self.userId = userId
        self.moduleId = moduleId
        self.template = self.db.wizard_configuration.find({"_id":ObjectId(self.moduleId)})[0]
        self.userconf = self.__class__.users[userId]
     
    def createConfiguration(self):
        templates = self.__getTemplatesIds() 

        kpis =[]
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
                kpis.append(tempId)
               
                
        self.__getWebProfilingOozie(kpis)
    
        self.setCoordination()
        self.save()   
    
    def __getWebProfilingOozie(self,kpis):
        if kpis.__len__() > 1:
            for kpiId in kpis:
                filename = "WP_" + kpiId + ".xml"
                self.__addMultipleWebProfilingNode(kpiId, filename)
        else:
            filename = "WP_" + kpis.__getitem__(0) + ".xml"
            self.__addSingleWebProfilingNode(kpis.__getitem__(0), filename)
            
        
    def __getTemplatesIds(self):
        values = self.db.wizard_configuration.find({"_id":ObjectId(self.moduleId)})[0]['templates']
        return values
    
    def __getTemplateById(self, templateId):
        template = self.db.wizard_template.find({"_id":ObjectId(templateId)})[0]
        return template
    
    def __addFilteringNode(self, filePath):
        child = self.xmlWorkflow.createElement("xi:include")
        child.setAttribute('href', filePath)
        refPath = "//fork[@name='aggregation']"
        refNode = xpath.findnode(refPath, self.xmlWorkflow)
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        
    def __addMultipleWebProfilingNode(self, name, filePath):
        child = self.xmlWorkflow.createElement("path")
        child.setAttribute('start', 'action_' + name)
        path = "//fork[@name='aggregation']"
        refNode = xpath.findnode(path, self.xmlWorkflow)
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        aggFork = xpath.findnode(path, self.xmlWorkflow)
        aggFork.nodeType = Node.ELEMENT_NODE
        aggFork.appendChild(child)
        child = self.xmlWorkflow.createElement("xi:include")
        child.setAttribute('href', filePath)
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        self.__createWebProfilingXML(name, True)
        
    def __createWebProfilingXML(self, templateId, multi):
        wp = WebProfiling()
        if multi == True:
            wp.setName(templateId)
        else:
            wp.setNext("end")
        wb = wp.getOoziesXml(self.__getTemplateById(templateId), self.userconf, self.template)  
        fileName = "WP_" + templateId + ".xml"
        f2 = open("./" +  fileName,"w")
        f2.write(wb)
        f2.close()
        
    def __addSingleWebProfilingNode(self, name, filePath):
        refPath = "//fork[@name='aggregation']"
        refNode = xpath.findnode(refPath, self.xmlWorkflow)
        parent = refNode.parentNode
        parent.removeChild(refNode)
        refPath = "//join[@name='joining']"
        refNode = xpath.findnode(refPath, self.xmlWorkflow)
        parent = refNode.parentNode
        parent.removeChild(refNode)
        child = self.xmlWorkflow.createElement("xi:include")
        child.setAttribute('href', filePath)
        refPath = "//kill[@name='fail']"
        refNode = xpath.findnode(refPath, self.xmlWorkflow)
        self.xmlWorkflow.firstChild.insertBefore(child, refNode)
        self.__createWebProfilingXML(name, False)
        
        
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
        refNode.firstChild.data = nameNode +  self.__class__.paths['configuration'] + self.userId + "/" + self.template['name']
        
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
        os.system("hadoop dfs -rmr " + self.__class__.paths['configuration'] + self.userId + "/" + self.template['name'])
        os.system("hadoop dfs -mkdir " + self.userconf['defaultPath'] + self.userconf['modules'][self.moduleId]['landing'])
        os.system("hadoop dfs -put ./coordinator.xml " + self.__class__.paths['configuration'] + self.userId + "/" + self.template['name'] + "/coordinator.xml")
        os.system("hadoop dfs -put ./workflow.xml " + self.__class__.paths['configuration'] + self.userId + "/" + self.template['name'] + "/workflow.xml")
        os.system("rm -f *.xml")
        os.system("oozie job -run -D oozie.coord.application.path=hdfs://pshdp01:8011" + self.__class__.paths['configuration'] + self.userId + "/" + self.template['name'])