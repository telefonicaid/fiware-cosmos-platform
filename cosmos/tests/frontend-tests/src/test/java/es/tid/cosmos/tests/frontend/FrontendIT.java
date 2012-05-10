package es.tid.cosmos.tests.frontend;

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
import org.testng.annotations.Parameters;
import org.testng.annotations.Test;

import es.tid.cosmos.tests.frontend.om.*;
import es.tid.cosmos.tests.hadoopjars.HadoopJars;
import es.tid.cosmos.tests.hadoopjars.JarNames;
import es.tid.cosmos.tests.tasks.Environment;
import es.tid.cosmos.tests.tasks.Task;
import es.tid.cosmos.tests.tasks.TaskStatus;
import es.tid.cosmos.tests.tasks.TestException;

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
                if (link.getText().equalsIgnoreCase("home")) {
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

        Task task = FrontEndTask.createFromExistingTaskId(
                this.frontend.getEnvironment(),
                taskId);
        task.run();
        task.waitForCompletion();
        assertEquals(task.getStatus(),
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
        Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                     inputFilePath,
                                     jarName);
        task.run();
        task.waitForCompletion();
        assertEquals(task.getStatus(),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. Task: " + task);
        task.getResults();        // Just verifying results can be accessed
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
        Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                     inputFilePath,
                                     this.wordcountJarPath);
        task.run();
        task.waitForCompletion();
        assertEquals(task.getStatus(),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. Task: " + task);
        task.getResults();     // Just verifying results can be accessed
    }

    public void testParallelTasks() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        Environment environment = this.frontend.getEnvironment();
        Task[] tasks = new Task[TASK_COUNT];
        for (int i = 0; i < TASK_COUNT; ++i) {
            tasks[i] = new FrontEndTask(environment,
                                        inputFilePath,
                                        this.wordcountJarPath);
        }
        for (Task task : tasks) {
            assertEquals(TaskStatus.Created,
                         task.getStatus(),
                         "Veryfing task is in created state. Task: " + task);
        }
        for (Task task : tasks) {
            task.run();
        }
        for (Task task : tasks) {
            TaskStatus jobStatus = task.getStatus();
            assertTrue(jobStatus == TaskStatus.Completed
                    || jobStatus == TaskStatus.Running,
                       "Verifying task is in running or completed state."
                    + " Task: " + task);
        }
        for (Task task : tasks) {
            task.waitForCompletion();
        }
        for (Task task : tasks) {
            TaskStatus jobStatus = task.getStatus();
            assertEquals(jobStatus,
                         TaskStatus.Completed,
                         "Verifying task is in completed state."
                    + " Task: " + task);
        }
        for (Task task : tasks) {
            // Just verifying results can be accessed
            task.getResults();
        }
    }

    public void testInvalidJar() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputFilePath,
                                           this.invalidJarPath);
        assertEquals(TaskStatus.Created,
                     task.getStatus(),
                     "Veryfing task is in created state. Task: " + task);
        task.run();
        TaskStatus jobStatus = task.getStatus();
        assertTrue(jobStatus == TaskStatus.Error
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or error state."
                + " Task: " + task);
        task.waitForCompletion();
        assertTrue(task.getStatus() == TaskStatus.Error,
                   "Verifying task is in error state."
                + " Task: " + task);
        verifyLinks();
    }

    public void testFailureJar() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputFilePath,
                                           this.mapperFailJarPath);
        assertEquals(TaskStatus.Created,
                     task.getStatus(),
                     "Veryfing task is in created state. Task: " + task);
        task.run();
        TaskStatus jobStatus = task.getStatus();
        assertTrue(jobStatus == TaskStatus.Error
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or error state."
                + " Task: " + task);
        task.waitForCompletion();
        assertTrue(task.getStatus() == TaskStatus.Error,
                   "Verifying task is in error state."
                + " Task: " + task);
        verifyLinks();
    }

    public void testListResultJar() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(
                "2 3 4 5 6 7 8 9 123\n19283");
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputFilePath,
                                           this.printPrimesJarPath);
        assertEquals(TaskStatus.Created,
                     task.getStatus(),
                     "Veryfing task is in created state. Task: " + task);
        task.run();
        TaskStatus jobStatus = task.getStatus();
        assertTrue(jobStatus == TaskStatus.Completed
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or completed state."
                + " Task: " + task);
        task.waitForCompletion();
        assertTrue(task.getStatus() == TaskStatus.Completed,
                   "Verifying task is in the completed state."
                + " Task: " + task);
        verifyLinks();
        List<Map<String, String>> results = task.getResults();
        assertEquals(results.get(0).get("primes"),
                     "[2, 3, 5, 7]",
                     "Verifying result is correct");

    }

    public void testDotsInName() throws IOException, TestException {
        final String inputFilePath = createAutoDeleteFile(
                "2 3 4 5 6 7 8 9 123\n19283");
        final String taskIdPrefix = "../1|<b>W</b>._1!!&nbsp;%20~€";
        String taskId = taskIdPrefix + UUID.randomUUID().toString().substring(25);
        Task task = new FrontEndTask(new FrontEnd(this.frontend.getEnvironment(),
                                                  "test@.2",
                                                  "cosmostest@.2"),
                                     inputFilePath,
                                     this.printPrimesJarPath,
                                     taskId);
        task.run();
        task.waitForCompletion();
        assertTrue(task.getStatus() == TaskStatus.Completed,
                   "Verifying task is in the completed state."
                + " TaskId: " + taskId);
        verifyLinks();
        List<Map<String, String>> results = task.getResults();
    }
}
