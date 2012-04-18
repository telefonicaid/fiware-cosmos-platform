package es.tid.cosmos.tests.tasks;

import java.util.List;
import java.util.Map;

/**
 *
 * @author ximo
 */
public interface Task {
    /**
     * This function will start the execution of the task in the cluster
     */
    void run() throws TestException;
    
    /**
     * This function blocks until the task finished execution (i.e. it enters
     * the Error or Completed state).
     */
    void waitForCompletion() throws TestException;
    
    /**
     * This function returns the task's status
     */
    TaskStatus getStatus() throws TestException;
    
    /**
     * If the task is in the Completed state, this function returns a
     * representation of the MongoDB output of that task. Otherwise, it throws
     * a TestException.
     */
    List<Map<String,String>> getResults() throws TestException;
}
