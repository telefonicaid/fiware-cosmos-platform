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

import es.tid.cosmos.tests.frontend.om.CreateJobPage;
import es.tid.cosmos.tests.frontend.om.FrontEnd;
import es.tid.cosmos.tests.frontend.om.FrontEndTask;
import es.tid.cosmos.tests.hadoopjars.HadoopJars;
import es.tid.cosmos.tests.hadoopjars.JarNames;
import es.tid.cosmos.tests.tasks.Environment;
import es.tid.cosmos.tests.tasks.Task;
import es.tid.cosmos.tests.tasks.TaskStatus;

/**
 *
 * @author ximo
 */
@Test(singleThreaded = true)
public class FrontendIT {
    private static final String SIMPLE_TEXT = "Very simple text file";
    private static final int TASK_COUNT = 4;
    private FrontEnd frontend;
    private String wordcountHdfsPath;
    private String mapperFailHdfsPath;
    private String printPrimesHdfsPath;
    private String invalidJarHdfsPath;

    @Parameters("environment")
    @BeforeClass
    public void setUp(String environment) throws IOException {
        this.frontend = new FrontEnd(Environment.valueOf(environment));
        this.wordcountHdfsPath = FrontendIT.ensureJar(
                this.frontend, FrontEnd.DEFAULT_USER,
                HadoopJars.getPath(JarNames.Wordcount));
        this.mapperFailHdfsPath = FrontendIT.ensureJar(
                this.frontend, FrontEnd.DEFAULT_USER,
                HadoopJars.getPath(JarNames.MapperFail));
        this.printPrimesHdfsPath = FrontendIT.ensureJar(
                this.frontend, FrontEnd.DEFAULT_USER,
                HadoopJars.getPath(JarNames.PrintPrimes));
        this.invalidJarHdfsPath = FrontendIT.ensureJar(
                this.frontend, FrontEnd.DEFAULT_USER,
                this.createAutoDeleteFile("Invalid Jar"));
    }

    private static String ensureJar(FrontEnd frontend, String user, String localPath) {
        if (!FrontEndTask.jarExists(frontend.getEnvironment(), user, localPath)) {
            FrontEndTask.uploadJar(frontend, localPath);
        }

        return FrontEndTask.getJarHdfsPath(user, localPath);
    }

    private static boolean isLive(String link) {
        HttpURLConnection urlConnection = null;
        try {
            URL url = new URL(link);
            urlConnection = (HttpURLConnection)url.openConnection();
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
            }
        } catch (MalformedURLException ex) {
            fail("Malformed URL in page "
                    + this.frontend.getDriver().getCurrentUrl()
                    + ". Info: " + ex.toString());
        }
    }

    public void testMainPage() {
        this.frontend.gotoCosmosHome();
        verifyLinks();
    }

    public void testNoNameFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        String currentUrl = driver.getCurrentUrl();
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);

        verifyLinks();
        createJobPage.setInputFile(inputFilePath);
        createJobPage.setInputJar(this.wordcountHdfsPath);
        createJobPage.create();

        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
        assertFalse(this.frontend.taskExists(""),
                    "Verify task hasn't been created");
        assertFalse(this.frontend.taskExists("null"),
                    "Verify task hasn't been created");
        assertFalse(this.frontend.taskExists("None"),
                    "Verify task hasn't been created");
    }

    public void testNoJarFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        final String taskId = UUID.randomUUID().toString();
        createJobPage.setName(taskId);
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        createJobPage.setInputFile(inputFilePath);

        String currentUrl = driver.getCurrentUrl();
        createJobPage.create();

        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
        assertFalse(this.frontend.taskExists(taskId),
                    "Verify task hasn't been created. TaskId: " + taskId);
    }

    public void testNoInputFile() throws IOException {
        final String taskId = UUID.randomUUID().toString();

        // Create job without data and verify we get an error if no data
        // is specified
        WebDriver driver = this.frontend.getDriver();
        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        createJobPage.setName(taskId);
        createJobPage.setInputJar(this.wordcountHdfsPath);
        String currentUrl = driver.getCurrentUrl();
        createJobPage.create();

        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
    }

    public void verifySampleJarFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        verifyLinks();

        // Get JAR link
        WebElement jarLink = driver.findElement(
                By.id(CreateJobPage.SAMPLE_JAR_LINK_ID));
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
        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        String restrictions = createJobPage.getJarRestrictions();
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

    public void testSimpleTask() throws IOException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                     inputFilePath,
                                     this.wordcountHdfsPath);
        task.run();
        task.waitForCompletion();
        assertEquals(task.getStatus(),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. Task: " + task);
        task.getResults();     // Just verifying results can be accessed
    }

    public void testParallelTasks() throws IOException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        Environment environment = this.frontend.getEnvironment();
        Task[] tasks = new Task[TASK_COUNT];
        for (int i = 0; i < TASK_COUNT; ++i) {
            tasks[i] = new FrontEndTask(environment,
                                        inputFilePath,
                                        this.wordcountHdfsPath);
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

    public void testInvalidJar() throws IOException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputFilePath,
                                           this.invalidJarHdfsPath);
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

    public void testFailureJar() throws IOException {
        final String inputFilePath = createAutoDeleteFile(SIMPLE_TEXT);
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputFilePath,
                                           this.mapperFailHdfsPath);
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

    public void testListResultJar() throws IOException {
        final String inputFilePath = createAutoDeleteFile(
                "2 3 4 5 6 7 8 9 123\n19283");
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputFilePath,
                                           this.printPrimesHdfsPath);
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

    public void testDotsInName() throws IOException {
        final String inputFilePath = createAutoDeleteFile(
                "2 3 4 5 6 7 8 9 123\n19283");
        final String taskIdPrefix = "../1|<b>W</b>._1!!&nbsp;%20~€";
        final String user = "test@.2";
        final String password = "cosmostest@.2";
        String taskId = taskIdPrefix + UUID.randomUUID().toString().substring(25);
        FrontEnd front = new FrontEnd(this.frontend.getEnvironment(),
                                      user, password);
        String jarHdfsPath = FrontendIT.ensureJar(
                front, user, HadoopJars.getPath(JarNames.PrintPrimes));
        Task task = new FrontEndTask(front, inputFilePath, jarHdfsPath,
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
