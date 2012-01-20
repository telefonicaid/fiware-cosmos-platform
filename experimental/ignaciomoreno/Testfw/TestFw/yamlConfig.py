"""
 YAML Configuration Module
"""

import logging, yaml

class YamlConfig(object):
    """
    """
    def __init__(self, yamlFile = None):
        """
        Constructor.
        """
        self.yamlFile = yamlFile
        self.yamlDict = yaml.load(file(yamlFile))

    def __del__(self):
        """
        Destructor
        """
        #Close file

    def getFile(self):
        """
        Getter method for node

        \return: The dictionary containing the YAML File.
        \rtype: str
        """
        return self.yamlFile

    def getDict(self):
        """
        """
        return self.yamlDict

    def loadYaml(self, yamlFile):

        if self.getFile() == None:
            self.yamlDict = yaml.load(file(yamlFile))
        else:
            pass


def TesterMethod():
    yamlConfigExample = YamlConfig('./moduleTestData/YamlFile.yml')

    print(yamlConfigExample.getFile())
    print(yamlConfigExample.getDict())

if __name__ == "__main__":
    TesterMethod()



"""
http://pyyaml.org/wiki/LibYAML
http://pyyaml.org/wiki/PyYAMLDocumentation
http://pyyaml.org/wiki/PyYAMLDocumentation#Constructorsrepresentersresolvers
http://docs.python.org/reference/datamodel.html
"""

