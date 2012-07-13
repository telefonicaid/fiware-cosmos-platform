package es.tid.cosmos.tests.tasks;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * @author ximo
 */
public abstract class Task {
    
    private static final int SLEEP_TIME = 30000; // 30 seconds
    protected Map<String, String> parameters = new HashMap<String, String>();
    protected String dataSet;

    /**
     * This function will start the execution of the task in the cluster
     */
    public abstract void run();

    public void setParameter(String paramName, String value) {
        this.parameters.put(paramName, value);
    }
    
    public void setDataset(String path) {
        this.dataSet = path;
    }

    /**
     * This function blocks until the task finished execution (i.e. it enters
     * the Error or Completed state).
     */
    public void waitForCompletion() {
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
    public abstract TaskStatus getStatus();

    /**
     * If the task is in the Completed state, this function returns a
     * representation of the MongoDB output of that task. Otherwise, it throws a
     * TestException.
     */
    public abstract List<Map<String, String>> getResults();
}
