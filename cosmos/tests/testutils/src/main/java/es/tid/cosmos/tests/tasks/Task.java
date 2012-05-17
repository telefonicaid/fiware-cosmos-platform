package es.tid.cosmos.tests.tasks;

import java.util.List;
import java.util.Map;

/**
 *
 * @author ximo
 */
public abstract class Task {
    private static final int SLEEP_TIME = 30000; // 30 seconds

    /**
     * This function will start the execution of the task in the cluster
     */
    public abstract void run() throws TestException;

    /**
     * This function blocks until the task finished execution (i.e. it enters
     * the Error or Completed state).
     */
    public void waitForCompletion() throws TestException {
        if (this.getStatus() == TaskStatus.Created) {
            this.run();
        }
        while (true) {
            if (this.getStatus() != TaskStatus.Running) {
                break;
            }
            try {
                Thread.sleep(SLEEP_TIME);
            } catch (InterruptedException ex) {
                throw new TestException("[InterruptedException]", ex);
            }
        }
    }

    /**
     * This function returns the task's status
     */
    public abstract TaskStatus getStatus() throws TestException;

    /**
     * If the task is in the Completed state, this function returns a
     * representation of the MongoDB output of that task. Otherwise, it throws a
     * TestException.
     */
    public abstract List<Map<String, String>> getResults() throws TestException;
}
