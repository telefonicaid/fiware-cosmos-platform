from yamlConfig import *

class TestSuite(YamlConfig):
    """
    """

    def __init__(self, A = None):
        """
        """
        pass
        print "hola"
        YamlConfig.__init__(self, A)

    def __del__(self):
        """
        """
        pass

def TesterMethod():
    testSuite = TestSuite('./moduleTestData/YamlFile.yml')
    pass

if __name__ == '__main__':
    TesterMethod()
