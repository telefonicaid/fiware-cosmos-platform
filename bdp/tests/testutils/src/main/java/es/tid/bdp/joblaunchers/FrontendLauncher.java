package es.tid.bdp.joblaunchers;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import static org.testng.Assert.assertEquals;

import es.tid.bdp.frontend.om.*;

/**
 *
 * @author ximo
 */
public class FrontendLauncher implements JobLauncher {
    private static final int SLEEP_TIME = 30000; // 30 seconds
    private FrontEnd frontend;

    public FrontendLauncher() {
        this.frontend = new FrontEnd();
    }

    public FrontendLauncher(String username, String password) {
        this.frontend = new FrontEnd(username, password);
    }

    @Override
    public String createNewTask(String inputFilePath, String jarPath)
            throws TestException {
        return this.createNewTask(inputFilePath, jarPath, true);
    }

    public String createNewTask(String inputFilePath,
                                String jarPath,
                                boolean shouldLaunch)
            throws TestException {
        final String taskId = UUID.randomUUID().toString();
        createNewTask(inputFilePath, jarPath, taskId, shouldLaunch);
        return taskId;
    }

    public void createNewTask(String inputFilePath,
                              String jarPath,
                              String jobName,
                              boolean shouldLaunch)
            throws TestException {
        // Verify input params
        if (!(new File(inputFilePath).exists())) {
            throw new TestException("Input path does not exist");
        }
        if (!(new File(jarPath).exists())) {
            throw new TestException("JAR path does not exist.");
        }

        // Set name
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        namePage.setName(jobName);

        // Set snd submit JAR
        SelectJarPage jarPage = namePage.submitNameForm();
        jarPage.setInputJar(jarPath);
        SelectInputPage inputPage = jarPage.submitJarFileForm();

        // Set and submit input file
        inputPage.setInputFile(inputFilePath);
        inputPage.submitInputFileForm();

        if (shouldLaunch) {
            this.launchTask(jobName);
        }
    }

    public void launchTask(String taskId) throws TestException {
        assertEquals(getTaskStatus(taskId),
                     TaskStatus.Created,
                     "Verifying task is in Created state");
        this.frontend.launchJob(taskId);
    }

    @Override
    public void waitForTaskCompletion(String taskId) throws TestException {
        while (true) {
            // Go to the main page
            this.frontend.goHome();
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
    public List<Map<String, String>> getResults(String taskId)
            throws TestException {
        ResultsPage resultsPage = this.frontend.goToResultsPage(taskId);
        return resultsPage.getResults();
    }

    @Override
    public TaskStatus getTaskStatus(String taskId) throws TestException {
        return this.frontend.getTaskStatus(taskId);
    }
}
