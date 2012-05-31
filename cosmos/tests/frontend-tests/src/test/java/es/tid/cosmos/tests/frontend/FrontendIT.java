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

import es.tid.cosmos.tests.environment.Environment;
import es.tid.cosmos.tests.frontend.om.*;
import es.tid.cosmos.tests.hadoopjars.HadoopJars;
import es.tid.cosmos.tests.hadoopjars.JarNames;
import es.tid.cosmos.tests.tasks.FrontEndTask;
import es.tid.cosmos.tests.tasks.Task;
import es.tid.cosmos.tests.tasks.TaskStatus;

/**
 *
 * @author ximo
 */
@Test(singleThreaded = true)
public class FrontendIT {
    private static final String SIMPLE_TEXT = "Very simple text file";
    private static final String WHITESPACE_TEXT = "   \t\n    ";
    private static final String PRIMES_TEXT = "2 3 4 5 6 7 8 9 123\n19283";
    private static final int TASK_COUNT = 4;
    private FrontEnd frontend;
    private String wordcountHdfsPath;
    private String mapperFailHdfsPath;
    private String printPrimesHdfsPath;
    private String invalidJarHdfsPath;
    private String simpleTextHdfsPath;
    private String whitespaceTextHdfsPath;

    @Parameters("environment")
    @BeforeClass
    public void setUp(String environment) throws IOException {
        this.frontend = new FrontEnd(Environment.valueOf(environment));
        this.wordcountHdfsPath = this.ensureJar(
                HadoopJars.getPath(JarNames.Wordcount));
        this.mapperFailHdfsPath = this.ensureJar(
                HadoopJars.getPath(JarNames.MapperFail));
        this.printPrimesHdfsPath = this.ensureJar(
                HadoopJars.getPath(JarNames.PrintPrimes));
        this.invalidJarHdfsPath = this.ensureJar(
                FrontendIT.createAutoDeleteFile("Invalid Jar", ".jar"));
        this.simpleTextHdfsPath = this.ensureData(
                FrontendIT.createAutoDeleteFile(SIMPLE_TEXT));
        this.whitespaceTextHdfsPath = this.ensureData(
                FrontendIT.createAutoDeleteFile(WHITESPACE_TEXT));
    }

    private static String ensureJar(FrontEnd frontend, String localPath) {
        final String fileName = new File(localPath).getName();
        if (!FrontEndTask.jarExists(frontend, fileName)) {
            FrontEndTask.uploadJar(frontend, localPath);
        }

        return FrontEndTask.getJarHdfsPath(frontend.getUsername(), fileName);
    }

    private String ensureJar(String localPath) {
        return FrontendIT.ensureJar(this.frontend, localPath);
    }

    private static String ensureData(FrontEnd frontend, String localPath) {
        final String fileName = new File(localPath).getName();
        if (!FrontEndTask.dataSetExists(frontend, fileName)) {
            FrontEndTask.uploadData(frontend, localPath);
        }

        return FrontEndTask.getDataHdfsPath(frontend.getUsername(), fileName);
    }

    private String ensureData(String localPath) {
        return FrontendIT.ensureData(this.frontend, localPath);
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

    public void verifyFeedbackLink() {
        this.frontend.gotoHueHome();
        WebDriver driver = this.frontend.getDriver();
        WebElement link = driver.findElement(By.id("hue-feedback"));
        assertEquals(link.getAttribute("href"), "mailto:cosmos@tid.es");
    }

    private void verifyLinks() {
        WebDriver driver = this.frontend.getDriver();
        List<WebElement> links = driver.findElements(By.tagName("a"));
        try {
            for (WebElement link : links) {
                String verbatimUrl = link.getAttribute("href");
                if (verbatimUrl == null || verbatimUrl.startsWith("javascript")) {
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

    public void testUploadJarNoFile() {
        UploadPage uploadPage = this.frontend.goToUpload();
        UploadJarPage uploadJarPage = uploadPage.goToUploadJar();
        uploadJarPage.submitForm();
        assertTrue(uploadJarPage.getFormErrorText().contains("required"),
                   "Verifying page errors if no file is submitted");
    }

    public void testUploadJarExistingFile() throws IOException {
        String localTmpFile = FrontendIT.createAutoDeleteFile(SIMPLE_TEXT);
        this.ensureJar(localTmpFile);
        UploadPage uploadPage = this.frontend.goToUpload();
        verifyLinks();
        UploadJarPage uploadJarPage = uploadPage.goToUploadJar();
        verifyLinks();
        uploadJarPage.setName(new File(localTmpFile).getName());
        uploadJarPage.setJarFile(localTmpFile);
        uploadJarPage.submitForm();
        assertTrue(uploadJarPage.getErrorText().contains("rename"),
                   "Verifying page errors if file already exists");
    }

    public void testUploadJarSubDirectory() throws IOException {
        String localTmpFile = FrontendIT.createAutoDeleteFile(SIMPLE_TEXT);
        UploadPage uploadPage = this.frontend.goToUpload();
        verifyLinks();
        UploadJarPage uploadJarPage = uploadPage.goToUploadJar();
        verifyLinks();
        uploadJarPage.setName("sub/" + new File(localTmpFile).getName());
        uploadJarPage.setJarFile(localTmpFile);
        uploadJarPage.submitForm();
        assertTrue(uploadJarPage.getErrorText().contains("rename"),
                   "Verifying page errors if file already exists");
    }

    public void testUploadDataNoFile() {
        UploadPage uploadPage = this.frontend.goToUpload();
        UploadDataPage uploadDataPage = uploadPage.goToUploadData();
        uploadDataPage.submitForm();
        assertTrue(uploadDataPage.getFormErrorText().contains("required"),
                   "Verifying page errors if no file is submitted");
    }

    public void testUploadDataExistingFile() throws IOException {
        String localTmpFile = FrontendIT.createAutoDeleteFile(SIMPLE_TEXT);
        this.ensureData(localTmpFile);
        UploadPage uploadPage = this.frontend.goToUpload();
        verifyLinks();
        UploadDataPage uploadDataPage = uploadPage.goToUploadData();
        uploadDataPage.setName(new File(localTmpFile).getName());
        verifyLinks();
        uploadDataPage.setDataFile(localTmpFile);
        uploadDataPage.submitForm();
        assertTrue(uploadDataPage.getErrorText().contains("rename"),
                   "Verifying page errors if file already exists");
    }

    public void testNoNameFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        String currentUrl = driver.getCurrentUrl();

        verifyLinks();
        createJobPage.setInputFile(this.simpleTextHdfsPath);
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
        createJobPage.setInputFile(this.simpleTextHdfsPath);

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
        String hdfsSamplePath = FrontendIT.ensureJar(this.frontend, jarName);
        Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                     this.simpleTextHdfsPath,
                                     hdfsSamplePath);
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

    private static String createAutoDeleteFile(String text, String extension)
            throws IOException {
        File tmpFile = File.createTempFile("webui-", extension);
        tmpFile.deleteOnExit();

        PrintWriter writer = new PrintWriter(tmpFile);
        try {
            writer.write(text);
        } finally {
            writer.close();
        }

        return tmpFile.getAbsolutePath();
    }

    private static String createAutoDeleteFile(String text)
            throws IOException {
        return createAutoDeleteFile(text, ".tmp");
    }

    public void testSimpleTask() throws IOException {
        Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                     this.simpleTextHdfsPath,
                                     this.wordcountHdfsPath);
        task.run();
        task.waitForCompletion();
        assertEquals(task.getStatus(),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. Task: " + task);
        task.getResults();     // Just verifying results can be accessed
    }

    public void testNoResultsTask() throws IOException {
        Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                     this.whitespaceTextHdfsPath,
                                     this.wordcountHdfsPath);
        task.run();
        task.waitForCompletion();
        assertEquals(task.getStatus(),
                     TaskStatus.Completed,
                     "Verifying task completed successfully. Task: " + task);
        List<Map<String, String>> results = task.getResults();
        assertEquals(results.size(), 0,
                     "Verifying no results have been generated");
    }

    public void testParallelTasks() throws IOException {
        Environment environment = this.frontend.getEnvironment();
        Task[] tasks = new Task[TASK_COUNT];
        for (int i = 0; i < TASK_COUNT; ++i) {
            tasks[i] = new FrontEndTask(environment,
                                        this.simpleTextHdfsPath,
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
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           this.simpleTextHdfsPath,
                                           this.invalidJarHdfsPath);
        task.run();
        TaskStatus jobStatus = task.getStatus();
        assertTrue(jobStatus == TaskStatus.Error
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or error state."
                + " Task: " + task);
        task.waitForCompletion();
        assertEquals(task.getStatus(), TaskStatus.Error,
                     "Verifying task is in error state."
                + " Task: " + task);
        verifyLinks();
    }

    public void testFailureJar() throws IOException {
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           this.simpleTextHdfsPath,
                                           this.mapperFailHdfsPath);
        task.run();
        TaskStatus jobStatus = task.getStatus();
        assertTrue(jobStatus == TaskStatus.Error
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or error state."
                + " Task: " + task);
        task.waitForCompletion();
        assertEquals(task.getStatus(), TaskStatus.Error,
                     "Verifying task is in error state."
                + " Task: " + task);
        verifyLinks();
    }

    public void testListResultJar() throws IOException {
        final String inputHdfsPath = this.ensureData(
                FrontendIT.createAutoDeleteFile(PRIMES_TEXT));
        final Task task = new FrontEndTask(this.frontend.getEnvironment(),
                                           inputHdfsPath,
                                           this.printPrimesHdfsPath);
        task.run();
        TaskStatus jobStatus = task.getStatus();
        assertTrue(jobStatus == TaskStatus.Completed
                || jobStatus == TaskStatus.Running,
                   "Verifying task is in running or completed state."
                + " Task: " + task);
        task.waitForCompletion();
        assertEquals(task.getStatus(), TaskStatus.Completed,
                     "Verifying task is in the completed state."
                + " Task: " + task);
        verifyLinks();
        List<Map<String, String>> results = task.getResults();
        assertEquals(results.get(0).get("primes"),
                     "[2, 3, 5, 7]",
                     "Verifying result is correct");

    }
}
