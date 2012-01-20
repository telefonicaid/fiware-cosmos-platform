from yamlConfig import *

class TestCase(YamlConfig):
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
    testCase = TestCase('./moduleTestData/YamlFile.yml')
    pass

if __name__ == '__main__':
    TesterMethod()
/Users/dmicol/Downloads/Testfw/TestFw/testrunscript.py