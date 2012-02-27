'''
Created on 22/02/2012

@author: javierb
'''
from es.tid.ps.config.preprocessing.preprocessing import Preprocessing
from es.tid.ps.config.webprofiling.webprofiling import WebProfiling
from es.tid.ps.config.configuration.configuration import Configuration
import os

config = Configuration()  
templates = config.getTemplatesIds("4f436f74367cfd07b1000003") 

for tempId in templates:
    temp = config.getTemplateById(tempId)
    if temp['template'] == "IngestionTemplate":
        print("Ingestion Template")
    elif temp['template'] == "PreProcessingTemplate":
        p = Preprocessing()
        b = p.getOoziesXml(temp)
        fileName = "PP_" + tempId + ".xml"
        f = open("./" + fileName ,"w")
        f.write(b)
        config.addFilteringNode(fileName)
    else:
        wp = WebProfiling()
        wb = wp.getOoziesXml(temp)  
        fileName = "WP_" + tempId + ".xml"
        f2 = open("./" +  fileName,"w")
        f2.write(wb)
        config.addWebProfilingNode(tempId, fileName)

config.save()
print os.system("dir C:\\")
os.system("xmllint --xinclude --format ./workflowout.xml > WebProfilingWorkflow.xml")
