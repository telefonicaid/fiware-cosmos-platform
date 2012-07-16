package es.tid.cosmos.tests.tasks;

import es.tid.cosmos.tests.environment.Environment;

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
    public Task createTask(String inputFilePath, String jarPath) {
        return new FrontEndTask(this.env, inputFilePath, jarPath);
    }
}
