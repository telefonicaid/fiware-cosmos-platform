package es.tid.cosmos.tests.frontend.om;

import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.htmlunit.HtmlUnitDriver;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

import es.tid.cosmos.tests.tasks.*;

/**
 *
 * @author ximo
 */
public class FrontEndTask implements Task {
    private static final int SLEEP_TIME = 30000; // 30 seconds
    private static final String FILE_BROWSER_URL = "TODO"; // TODO
    private boolean isRun;
    private final FrontEnd frontend;
    private final String taskId;
    private final String jarHdfsPath;
    private final String inputHdfsPath;

    public static String getJarHdfsPath(String user, String jarFileName) {
        return "/user/" + user + "/jars/" + jarFileName;
    }

    public static String getJarHdfsPath(String user) {
        return getJarHdfsPath(user, "");
    }

    public static String getDataHdfsPath(String user, String dataFileName) {
        return "/user/" + user + "/datasets/" + dataFileName;
    }

    public static String getDataHdfsPath(String user) {
        return getDataHdfsPath(user, "");
    }

    public static boolean jarExists(Environment env, String user,
                                    String jarName) {
        return fileExists(env, getJarHdfsPath(user), jarName);
    }

    public static boolean dataSetExists(Environment env, String user,
                                        String dataSetName) {
        return fileExists(env, getJarHdfsPath(user), dataSetName);
    }

    private static boolean fileExists(Environment env, String hdfsPath,
                                      String fileName) {
        WebDriver driver = new HtmlUnitDriver();
        driver.get(env.getProperty(EnvironmentSetting.FrontendUrl) + "/"
                + FILE_BROWSER_URL + hdfsPath);
        List<WebElement> files = driver.findElements(By.className("fb-file"));
        for (WebElement file : files) {
            if (file.getText().equals(fileName)) {
                return true;
            }
        }
        return false;
    }

    public static void uploadJar(FrontEnd frontend, String filePath) {
        UploadJarPage uploadJarPage = frontend.goToUploadJar();
        uploadJarPage.setJarFile(filePath);
        uploadJarPage.submitForm();
        throw new UnsupportedOperationException("Need to implement return value");
    }

    public static void uploadData(FrontEnd frontend, String filePath) {
        UploadDataPage uploadDataPage = frontend.goToUploadData();
        uploadDataPage.setDataFile(filePath);
        uploadDataPage.submitForm();
    }

    public FrontEndTask(FrontEnd frontend,
                        String inputHdfsPath,
                        String jarHdfsPath,
                        String taskId) {
        this.frontend = frontend;
        this.taskId = taskId;
        this.jarHdfsPath = jarHdfsPath;
        this.inputHdfsPath = inputHdfsPath;
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
        this.jarHdfsPath = null;
        this.inputHdfsPath = null;
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
        createJobPage.setName(this.taskId);
        createJobPage.setInputJar(this.jarHdfsPath);
        createJobPage.setInputFile(this.inputHdfsPath);
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
