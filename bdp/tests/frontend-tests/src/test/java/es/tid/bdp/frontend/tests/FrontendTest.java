package es.tid.bdp.frontend.tests;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.channels.Channels;
import java.nio.channels.ReadableByteChannel;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.*;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import es.tid.bdp.frontend.om.FrontEnd;
import es.tid.bdp.frontend.om.SelectInputPage;
import es.tid.bdp.frontend.om.SelectJarPage;
import es.tid.bdp.frontend.om.SelectNamePage;
import es.tid.bdp.joblaunchers.FrontendLauncher;
import es.tid.bdp.joblaunchers.TaskStatus;
import es.tid.bdp.joblaunchers.TestException;

/**
 *
 * @author ximo
 */
@Test(singleThreaded = true)
public class FrontendTest {
    private static final String WORDCOUNT_FILENAME = "wordcount.jar";
    private static final String MAPPERFAIL_FILENAME = "mapperfail.jar";
    private static final String PRINTPRIMES_FILENAME = "printprimes.jar";
    private static final String SIMPLE_TEXT = "Very simple text file";
    private static final int TASK_COUNT = 4;
    private FrontEnd frontend;
    private String wordcountJarPath;
    private String mapperFailJarPath;
    private String printPrimesJarPath;
    private String invalidJarPath;

    @BeforeClass
    public void setUp() throws IOException {
        this.frontend = new FrontEnd();
        File wordCountJarFile = new File(WORDCOUNT_FILENAME);
        this.wordcountJarPath = wordCountJarFile.getAbsolutePath();
        verifyFileExists(wordCountJarFile);

        File mapperFailJarFile = new File(MAPPERFAIL_FILENAME);
        this.mapperFailJarPath = mapperFailJarFile.getAbsolutePath();
        verifyFileExists(mapperFailJarFile);

        File printPrimesJarFile = new File(PRINTPRIMES_FILENAME);
        this.printPrimesJarPath = printPrimesJarFile.getAbsolutePath();
        verifyFileExists(printPrimesJarFile);

        this.invalidJarPath = createAutoDeleteFile("Invalid Jar");
    }

    private void verifyFileExists(File f) {
        assertTrue(f.exists(),
                   "Veryfing file is present: "
                + f.getAbsolutePath());
    }

    private static boolean isLive(String link) {
        HttpURLConnection urlConnection = null;
        try {
            URL url = new URL(link);
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.setRequestMethod("GET");
            urlConnection.connect();
            String redirectLink = urlConnection.getHeaderField("Location");
            if (redirectLink != null && !url.toString().equals(redirectLink)) {
                return isLive(redirectLink);
            } else {
                return urlConnection.getResponseCode()
                        == HttpURLConnection.HTTP_OK;
            }
        } catch (Exception e) {
            return false;
        } finally {
            if (urlConnection != null) {
                urlConnection.disconnect();
            }
        }
    }

    private void verifyLinks() {
        List<WebElement> links = this.frontend.getDriver().findElements(
                By.tagName("a"));
        try {
            for (WebElement link : links) {
                URL baseUrl = new URL(FrontEnd.HOME_URL);
                String verbatimUrl = link.getAttribute("href");
                if (verbatimUrl.startsWith("javascript")) {
                    return;
                }
                String linkUrl = new URL(baseUrl, verbatimUrl).toString();
                assertTrue(FrontendTest.isLive(linkUrl), "Broken link: " + linkUrl);
            }
        } catch (MalformedURLException ex) {
            fail("Malformed URL in page "
                    + this.frontend.getDriver().getCurrentUrl()
                    + ". Info: " + ex.toString());
        }
    }

    public void testMainPage() {
        this.frontend.goHome();
        verifyLinks();
    }

    public void testNoNameFile() {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        String currentUrl = driver.getCurrentUrl();

        verifyLinks();
        namePage.submitNameForm();

        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
        assertFalse(this.frontend.taskExists(""),
                    "Verify task hasn't been created");
        assertFalse(this.frontend.taskExists("null"),
                    "Verify task hasn't been created");
    }

    public void testNoJarFile() {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        final String taskId = UUID.randomUUID().toString();
        namePage.setName(taskId);
        SelectJarPage jarPage = namePage.submitNameForm();

        String currentUrl = driver.getCurrentUrl();
        verifyLinks();
        jarPage.submitJarFileForm();

        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
        assertFalse(this.frontend.taskExists(taskId),
                    "Verify task hasn't been created. TaskId: " + taskId);
    }

    public void testNoInputFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        final String taskId = UUID.randomUUID().toString();
        namePage.setName(taskId);

        SelectJarPage jarPage = namePage.submitNameForm();
        jarPage.setInputJar(this.invalidJarPath);

        SelectInputPage inputPage = jarPage.submitJarFileForm();
        String currentUrl = driver.getCurrentUrl();
        verifyLinks();
        inputPage.submitInputFileForm();

        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
    }

    public void verifySampleJarFile() throws IOException, TestException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        verifyLinks();

        // Get JAR link
        WebElement jarLink = driver.findElement(
                By.id(SelectNamePage.SAMPLE_JAR_LINK_ID));
        String verbatimUrl = jarLink.getAttribute("href");
        URL baseUrl = new URL(FrontEnd.HOME_URL);
        URL linkUrl = new URL(baseUrl, verbatimUrl);

        // Download file locally
        final String jarName = "sample.jar";
        ReadableByteChannel rbc = Channels.newChannel(linkUrl.openStream());
        FileOutputStream fos = new FileOutputStream(jarName);
        try {
            fos.getChannel().transferFrom(rbc, 0, 1 << 24);
        } finally {
            rbc.close();
            fos.close();
        }

        // Submit job with sample JAR
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        FrontendLauncher testDriver = new FrontendLauncher();
        String taskId = testDriver.createNewTask(inputFilePath, jarName);
        testDriver.waitForTaskCompletion(taskId);
        assertEquals(testDriver.getTaskStatus(taskId),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. TaskId: " + taskId);
        testDriver.getResults(taskId); // Just verifying results can be accessed        
    }

    public void verifyJarRestrictions() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        String restrictions = namePage.getJarRestrictions();
        assertTrue(
                restrictions.contains("Mongo"),
                "Verifying restrictions mention Mongo");
        assertTrue(
                restrictions.contains("Tool"),
                "Verifying restrictions mention the Tool interface");
    }

    private String createAutoDeleteFile(String text)
            throws IOException {
        File tmpFile = File.createTempFile("webui-", ".tmp");
        tmpFile.deleteOnExit();

        PrintWriter writer = new PrintWriter(tmpFile);
        try {
            writer.write(text);
        } finally {
            writer.close();
        }

        return tmpFile.getAbsolutePath();
    }

    public void testSimpleTask() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        FrontendLauncher testDriver = new FrontendLauncher();
        String taskId = testDriver.createNewTask(inputFilePath,
                                                 this.wordcountJarPath);
        testDriver.waitForTaskCompletion(taskId);
        assertEquals(testDriver.getTaskStatus(taskId),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. TaskId: " + taskId);
        testDriver.getResults(taskId); // Just verifying results can be accessed
    }

    public void testParallelTasks() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        FrontendLauncher testDriver = new FrontendLauncher();
        String[] taskIds = new String[TASK_COUNT];
        for (int i = 0; i < TASK_COUNT; ++i) {
            taskIds[i] = testDriver.createNewTask(inputFilePath,
                                                  this.wordcountJarPath,
                                                  false);
        }
        for (String taskId : taskIds) {
            assertEquals(TaskStatus.Created,
                         testDriver.getTaskStatus(taskId),
                         "Veryfing task is in created state. TaskId: "
                    + taskId);
        }
        for (String taskId : taskIds) {
            testDriver.launchTask(taskId);
        }
        for (String taskId : taskIds) {
            TaskStatus jobStatus = testDriver.getTaskStatus(taskId);
            assertTrue(jobStatus == TaskStatus.Completed
                    || jobStatus == TaskStatus.Running,
                       "Verifying task is in running or completed state."
                    + " TaskId: " + taskId);
        }
        for (String taskId : taskIds) {
            testDriver.waitForTaskCompletion(taskId);
        }
        for (String taskId : taskIds) {
            TaskStatus jobStatus = testDriver.getTaskStatus(taskId);
            assertEquals(jobStatus,
                         TaskStatus.Completed,
                         "Verifying task is in completed state."
                    + " TaskId: " + taskId);
        }
        for (String taskId : taskIds) {
            // Just verifying results can be accessed
            testDriver.getResults(taskId);
        }
    }

    public void testInvalidJar() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        FrontendLauncher testDriver = new FrontendLauncher();
        final String taskId = testDriver.createNewTask(inputFilePath,
                                                       this.invalidJarPath,
                                                       false);
        assertEquals(TaskStatus.Created,
                     testDriver.getTaskStatus(taskId),
                     "Veryfing task is in created state. TaskId: "
                + taskId);
        testDriver.launchTask(taskId);
        TaskStatus jobStatus = testDriver.getTaskStatus(taskId);
        assertTrue(jobStatus == TaskStatus.Error
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or error state."
                + " TaskId: " + taskId);
        testDriver.waitForTaskCompletion(taskId);
        assertTrue(testDriver.getTaskStatus(taskId) == TaskStatus.Error,
                   "Verifying task is in error state."
                + " TaskId: " + taskId);
        verifyLinks();
    }

    public void testFailureJar() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        FrontendLauncher testDriver = new FrontendLauncher();
        final String taskId = testDriver.createNewTask(inputFilePath,
                                                       this.mapperFailJarPath,
                                                       false);
        assertEquals(TaskStatus.Created,
                     testDriver.getTaskStatus(taskId),
                     "Veryfing task is in created state. TaskId: "
                + taskId);
        testDriver.launchTask(taskId);
        TaskStatus jobStatus = testDriver.getTaskStatus(taskId);
        assertTrue(jobStatus == TaskStatus.Error
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or error state."
                + " TaskId: " + taskId);
        testDriver.waitForTaskCompletion(taskId);
        assertTrue(testDriver.getTaskStatus(taskId) == TaskStatus.Error,
                   "Verifying task is in error state."
                + " TaskId: " + taskId);
        verifyLinks();
    }

    public void testListResultJar() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(
                "2 3 4 5 6 7 8 9 123\n19283");
        FrontendLauncher testDriver = new FrontendLauncher();
        final String taskId = testDriver.createNewTask(inputFilePath,
                                                       this.printPrimesJarPath,
                                                       false);
        assertEquals(TaskStatus.Created,
                     testDriver.getTaskStatus(taskId),
                     "Veryfing task is in created state. TaskId: "
                + taskId);
        testDriver.launchTask(taskId);
        TaskStatus jobStatus = testDriver.getTaskStatus(taskId);
        assertTrue(jobStatus == TaskStatus.Completed
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or completed state."
                + " TaskId: " + taskId);
        testDriver.waitForTaskCompletion(taskId);
        assertTrue(testDriver.getTaskStatus(taskId) == TaskStatus.Completed,
                   "Verifying task is in the completed state."
                + " TaskId: " + taskId);
        verifyLinks();
        List<Map<String, String>> results = testDriver.getResults(taskId);

    }

    public void testDotsInName() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(
                "2 3 4 5 6 7 8 9 123\n19283");
        FrontendLauncher testDriver = new FrontendLauncher("test@.2",
                                                           "cosmostest@.2");
        final String taskId = "../1234|<b>Weird</b>.Name_1!!&nbsp;%20~€";
        testDriver.createNewTask(inputFilePath,
                                 this.printPrimesJarPath,
                                 taskId,
                                 true);
        testDriver.waitForTaskCompletion(taskId);
        assertTrue(testDriver.getTaskStatus(taskId) == TaskStatus.Completed,
                   "Verifying task is in the completed state."
                + " TaskId: " + taskId);
        verifyLinks();
        List<Map<String, String>> results = testDriver.getResults(taskId);
    }
}
