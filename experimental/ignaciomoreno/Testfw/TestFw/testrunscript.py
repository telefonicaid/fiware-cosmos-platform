def main():
    """
    Tester method
    """
    testSuite = TestSuite("prototypeModule", "PrototypeTestSuite")
    testConfig = testSuite
    
    cmdImport = "from prototypeModule import PrototypeTestSuite" #%(variable)
    exec(cmdImport) in globals()
    
    #Los logs hay que definirlos aqui y despues ponerlos
    # como atributo del resto de clases (test suite, yaml, etc.)
    
    print(testConfig.getModule())
    #Check is loaded (need to decorade)#
    testSuiteModule = sys.modules[testConfig.getModule()]
    #print(testSuiteModule)
    #print(type(testSuiteModule))
          
    testSuiteClass = getattr(testSuiteModule, testSuite.getClass())
    print(testSuiteClass)
    print(type(testSuiteClass))
    testSuiteObject = testSuiteClass("hola", "adios", "hello")
    testSuiteObject.run



if __name__ == "__main__":
    main()
