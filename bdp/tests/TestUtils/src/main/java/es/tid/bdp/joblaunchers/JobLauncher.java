package es.tid.bdp.joblaunchers;

import java.util.Map;

/**
 *
 * @author ximo
 */
public interface JobLauncher {
    // Creates and launches a new Hadoop task that will use the input file
    // and JAR passed in. It returns a taskId value that can be used with the
    // other functions in the interface
    String createNewTask(String inputFilePath, String jarPath)
            throws TestException;
    
    // This function blocks until the taskId finished execution (i.e. it enters
    // the Error or Completed state).
    void waitForTaskCompletion(String taskId) throws TestException;
    
    // Given a taskId, this function returns the task's status
    TaskStatus getTaskStatus(String taskId) throws TestException;
    
    // Given a taskId that represents a task in the Completed state, this
    // function returns a representation of the MongoDB output of that task
    Map<String,String[]> getResults(String taskId) throws TestException;
}
