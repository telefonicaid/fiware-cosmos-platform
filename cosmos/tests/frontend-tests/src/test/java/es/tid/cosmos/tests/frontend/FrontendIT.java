package es.tid.cosmos.tests.frontend;

import java.io.*;
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
import org.testng.annotations.Parameters;
import org.testng.annotations.Test;

import es.tid.cosmos.tests.frontend.om.*;
import es.tid.cosmos.tests.hadoopjars.HadoopJars;
import es.tid.cosmos.tests.hadoopjars.JarNames;
import es.tid.cosmos.tests.joblaunchers.*;

/**
 *
 * @author ximo
 */
@Test(singleThreaded = true)
public class FrontendIT {
    private static final String SIMPLE_TEXT = "Very simple text file";
    private static final int TASK_COUNT = 4;
    private FrontEnd frontend;
    private String wordcountJarPath;
    private String mapperFailJarPath;
    private String printPrimesJarPath;
    private String invalidJarPath;

    @Parameters("environment")
    @BeforeClass
    public void setUp(String environment) throws IOException {
        this.wordcountJarPath = HadoopJars.getPath(JarNames.Wordcount); 
        this.mapperFailJarPath = HadoopJars.getPath(JarNames.MapperFail);
        this.printPrimesJarPath = HadoopJars.getPath(JarNames.PrintPrimes);
        this.invalidJarPath = createAutoDeleteFile("Invalid Jar");
        
        this.frontend = new FrontEnd(Environment.valueOf(environment));
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
        WebDriver driver = this.frontend.getDriver();
        List<WebElement> links = driver.findElements(By.tagName("a"));
        try {
            for (WebElement link : links) {
                String verbatimUrl = link.getAttribute("href");
                if (verbatimUrl.startsWith("javascript")) {
                    return;
                }
                String linkUrl = this.frontend.resolveURL(verbatimUrl).toString();
                assertTrue(FrontendIT.isLive(linkUrl),
                           "Broken link: " + linkUrl);
                if(link.getText().equalsIgnoreCase("home")) {
                    assertEquals(this.frontend.getHomeUrl(),
                                 linkUrl.toString());
                    
                }
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
    
    private void createNoInputFileJob(String taskId) {
        // Create job without data and verify we get an error if no data
        // is specified
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
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

    public void testNoInputFile() throws IOException, TestException {
        final String taskId = UUID.randomUUID().toString();        
        createNoInputFileJob(taskId);
        
        // Verify we can go back to the frontpage and upload data
        // through the "Upload Data" link
        SelectInputPage inputPage = this.frontend.setInputDataForJob(taskId);
        inputPage.setInputFile(this.invalidJarPath);
        inputPage.submitInputFileForm();
        
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
        testDriver.launchTask(taskId);
        testDriver.waitForTaskCompletion(taskId);
        assertEquals(testDriver.getTaskStatus(taskId),
                     TaskStatus.Error,
                     "Verifying task is in the error state");
    }

    public void verifySampleJarFile() throws IOException, TestException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        verifyLinks();

        // Get JAR link
        WebElement jarLink = driver.findElement(
                By.id(SelectNamePage.SAMPLE_JAR_LINK_ID));
        String verbatimUrl = jarLink.getAttribute("href");
        URL linkUrl = this.frontend.resolveURL(verbatimUrl);
        
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
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
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
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
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
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
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
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
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
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
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
        FrontendLauncher testDriver = new FrontendLauncher(
                this.frontend.getEnvironment());
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
