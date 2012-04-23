package es.tid.cosmos.tests.tasks;

import es.tid.cosmos.tests.frontend.om.FrontEndTask;

/**
 *
 * @author ximo
 */
public class FrontendTaskCreator implements TaskCreator {
    private final Environment env;
    
    public FrontendTaskCreator(Environment env) {
        this.env = env;
    }
    
    @Override
    public Task createTask(String inputFilePath, String jarPath)
            throws TestException {
        return new FrontEndTask(this.env, inputFilePath, jarPath);
    }
}
