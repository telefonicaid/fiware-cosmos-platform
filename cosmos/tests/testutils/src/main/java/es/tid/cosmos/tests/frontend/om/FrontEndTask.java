package es.tid.cosmos.tests.frontend.om;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import static org.testng.Assert.assertEquals;

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
    private final FrontEnd frontend;
    private final String taskId;

    public FrontEndTask(FrontEnd frontend,
                        String inputFilePath,
                        String jarPath,
                        String taskId) throws TestException {
        // Verify input params
        if (!(new File(inputFilePath).exists())) {
            throw new TestException("Input path does not exist");
        }
        if (!(new File(jarPath).exists())) {
            throw new TestException("JAR path does not exist.");
        }

        this.frontend = frontend;

        // Set name
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        this.taskId = taskId;
        namePage.setName(taskId);

        // Set and submit JAR
        SelectJarPage jarPage = namePage.submitNameForm();
        jarPage.setInputJar(jarPath);
        SelectInputPage inputPage = jarPage.submitJarFileForm();

        // Set and submit input file
        inputPage.setInputFile(inputFilePath);
        inputPage.submitInputFileForm();
    }

    public FrontEndTask(FrontEnd frontend,
                        String inputFilePath,
                        String jarPath) throws TestException {
        this(frontend, inputFilePath, jarPath, UUID.randomUUID().toString());
    }

    public FrontEndTask(Environment env, String inputFilePath, String jarPath)
            throws TestException {
        this(new FrontEnd(env), inputFilePath, jarPath);
    }

    public FrontEndTask(Environment env, String inputFilePath, String jarPath,
                        String taskId) throws TestException {
        this(new FrontEnd(env), inputFilePath, jarPath, taskId);
    }

    private FrontEndTask(Environment env, String taskId) throws TestException {
        this.taskId = taskId;
        this.frontend = new FrontEnd(env);
    }

    public static FrontEndTask CreateFromExistingTaskId(Environment env,
                                                        String taskId)
            throws TestException {
        return new FrontEndTask(env, taskId);
    }

    @Override
    public void run() throws TestException {
        assertEquals(this.getStatus(),
                     TaskStatus.Created,
                     "Verifying task is in Created state");
        this.frontend.launchJob(this.taskId);
    }

    @Override
    public void waitForCompletion() throws TestException {
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
    public TaskStatus getStatus() throws TestException {
        return this.frontend.getTaskStatus(taskId);
    }

    @Override
    public List<Map<String, String>> getResults()
            throws TestException {
        ResultsPage resultsPage = this.frontend.goToResultsPage(this.taskId);
        return resultsPage.getResults();
    }

    @Override
    public String toString() {
        return "[FrontEndTask] Id = " + taskId;
    }
}
