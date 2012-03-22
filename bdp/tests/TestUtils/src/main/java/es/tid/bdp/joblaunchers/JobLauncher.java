package es.tid.bdp.joblaunchers;

import java.util.Map;

/**
 *
 * @author ximo
 */
public interface JobLauncher {
    String createNewTask(String inputFilePath, String jarPath)
            throws TestException;
    void waitForTaskCompletion(String taskId) throws TestException;
    TaskStatus getTaskStatus(String taskId) throws TestException;
    Map<String,String[]> getResults(String taskId) throws TestException;
}
