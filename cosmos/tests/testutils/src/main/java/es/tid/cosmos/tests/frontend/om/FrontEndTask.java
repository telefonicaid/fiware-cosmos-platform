package es.tid.cosmos.tests.frontend.om;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

import es.tid.cosmos.tests.tasks.Environment;
import es.tid.cosmos.tests.tasks.Task;
import es.tid.cosmos.tests.tasks.TaskStatus;
import es.tid.cosmos.tests.tasks.TestException;

/**
 *
 * @author ximo
 */
public class FrontEndTask implements Task {
    private static final int SLEEP_TIME = 30000; // 30 seconds
    private boolean isRun;
    private final FrontEnd frontend;
    private final String taskId;
    private final String jarPath;
    private final String inputFilePath;

    public FrontEndTask(FrontEnd frontend,
                        String inputFilePath,
                        String jarPath,
                        String taskId) {
        // Verify input params
        if (!(new File(inputFilePath).exists())) {
            throw new TestException("Input path does not exist");
        }
        if (!(new File(jarPath).exists())) {
            throw new TestException("JAR path does not exist.");
        }

        this.frontend = frontend;
        this.taskId = taskId;
        this.jarPath = jarPath;
        this.inputFilePath = inputFilePath;
        this.isRun = false;
    }

    public FrontEndTask(FrontEnd frontend,
                        String inputFilePath,
                        String jarPath) {
        this(frontend, inputFilePath, jarPath, UUID.randomUUID().toString());
    }

    public FrontEndTask(Environment env, String inputFilePath, String jarPath) {
        this(new FrontEnd(env), inputFilePath, jarPath);
    }

    public FrontEndTask(Environment env, String inputFilePath, String jarPath,
                        String taskId) {
        this(new FrontEnd(env), inputFilePath, jarPath, taskId);
    }

    private FrontEndTask(Environment env, String taskId) {
        this.frontend = new FrontEnd(env);
        assertTrue(this.frontend.taskExists(taskId));

        this.taskId = taskId;
        this.isRun = true;
        this.jarPath = null;
        this.inputFilePath = null;
    }

    public static FrontEndTask CreateFromExistingTaskId(Environment env,
                                                        String taskId) {
        return new FrontEndTask(env, taskId);
    }

    @Override
    public void run() {
        assertFalse(this.isRun, "Veryfing run hasn't been called previously."
                + " If this fails, it is a test bug.");

        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        createJobPage.setName(taskId);
        createJobPage.setInputJar(jarPath);
        createJobPage.setInputFile(inputFilePath);
        createJobPage.create();
        this.isRun = true;
    }

    @Override
    public void waitForCompletion() {
        if (!this.isRun) {
            this.run();
        }
        while (true) {
            TaskStatus status = this.frontend.getTaskStatus(taskId);
            if (status != TaskStatus.Running) {
                break;
            }
            try {
                Thread.sleep(SLEEP_TIME);
            } catch (InterruptedException ex) {
                throw new TestException("[InterruptedException] "
                        + ex.getMessage());
            }
        }
    }

    @Override
    public TaskStatus getStatus() {
        return this.frontend.getTaskStatus(taskId);
    }

    @Override
    public List<Map<String, String>> getResults() {
        ResultsPage resultsPage = this.frontend.goToResultsPage(this.taskId);
        return resultsPage.getResults();
    }

    @Override
    public String toString() {
        return "[FrontEndTask] Id = " + taskId;
    }
}
