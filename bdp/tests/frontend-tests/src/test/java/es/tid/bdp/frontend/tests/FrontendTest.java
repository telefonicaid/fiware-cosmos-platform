package es.tid.bdp.frontend.tests;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;
import java.util.UUID;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.fail;

import es.tid.bdp.joblaunchers.FrontendLauncher;
import es.tid.bdp.joblaunchers.TaskStatus;
import es.tid.bdp.joblaunchers.TestException;
import es.tid.bdp.frontend.om.FrontEnd;
import es.tid.bdp.frontend.om.SelectInputPage;
import es.tid.bdp.frontend.om.SelectJarPage;
import es.tid.bdp.frontend.om.SelectNamePage;

/**
 *
 * @author ximo
 */
@Test(singleThreaded=true)
public class FrontendTest {
    private FrontEnd frontend;
    private String wordcountJarPath;
    private String emptyJarPath;
    
    @BeforeClass
    public void setUp() throws IOException {
        File wordCountJarFile = new File("wordcount.jar");
        this.wordcountJarPath = wordCountJarFile.getAbsolutePath();
        assertTrue(wordCountJarFile.exists(),
                   "Veryfing Wordcount Jar is present: "
                + this.wordcountJarPath);
        
        File tmpFile = File.createTempFile("empty", ".jar");
        tmpFile.deleteOnExit();
        this.emptyJarPath = tmpFile.getAbsolutePath();
    }
    
    public FrontendTest() {
        this.frontend = new FrontEnd();
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
            }
            else { 
                return urlConnection.getResponseCode()
                        == HttpURLConnection.HTTP_OK; 
            } 
        }
        catch (Exception e) { 
            return false; 
        }
        finally { 
            if (urlConnection != null) {
                urlConnection.disconnect();
            }
        } 
    }
    
    private void verifyLinks() throws MalformedURLException {
        List<WebElement> links = this.frontend.getDriver()
                .findElements(By.tagName("a"));       
        for(WebElement link : links) {
            URL baseUrl = new URL(FrontEnd.HOME_URL);
            String verbatimUrl = link.getAttribute("href");
            String linkUrl = new URL(baseUrl, verbatimUrl).toString();
            assertTrue(FrontendTest.isLive(linkUrl), "Broken link: " + linkUrl);
        }
    }
    
    public void testMainPage() throws MalformedURLException {
        this.frontend.goHome();
        verifyLinks();
    }
    
    public void testNoNameFile() throws MalformedURLException {
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
    
    public void testNoJarFile() throws MalformedURLException {
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
                    "Verify task hasn't been created");
    }
    
    public void testNoInputFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        final String taskId = UUID.randomUUID().toString();
        namePage.setName(taskId);
        
        SelectJarPage jarPage = namePage.submitNameForm();
        jarPage.setInputJar(this.emptyJarPath);        
        
        SelectInputPage inputPage = jarPage.submitJarFileForm();        
        String currentUrl = driver.getCurrentUrl();
        verifyLinks();        
        inputPage.submitInputFileForm();
        
        // We should be in the same page, and the form should be complaining
        assertEquals(currentUrl, driver.getCurrentUrl());
        driver.findElement(By.className("errorlist"));
        assertFalse(this.frontend.taskExists(taskId),
                    "Verify task hasn't been created");
    }
    
    public void verifySampleJarFile() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        namePage.setName(UUID.randomUUID().toString());
        
        SelectJarPage jarPage = namePage.submitNameForm();                
        // Just verifying it exists
        driver.findElement(By.id(SelectJarPage.SAMPLE_JAR_LINK_ID));
        verifyLinks();
    }
    
    public void verifyJarRestrictions() throws IOException {
        WebDriver driver = this.frontend.getDriver();
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        namePage.setName(UUID.randomUUID().toString());
        SelectJarPage jarPage = namePage.submitNameForm();
               
        String restrictions = driver.findElement(
                By.id(SelectJarPage.JAR_RESTRICTIONS_ID)).getText();        
        assertTrue(
                restrictions.contains("HDFS"),
                "Verifying restrictions mention HDFS");        
        assertTrue(
                restrictions.contains("Mongo"),
                "Verifying restrictions mention Mongo");        
        assertTrue(
                restrictions.contains("manifest"),
                "Verifying restrictions mention manifests");        
        assertTrue(
                restrictions.contains("Tool"),
                "Verifying restrictions mention the Tool interface");
    }
    
    public void testSimpleTask() throws IOException, TestException {
        final String inputFilePath;        
        {
            File tmpFile = File.createTempFile("webui-wordcount", ".tmp");
            tmpFile.deleteOnExit();
            
            PrintWriter writer = new PrintWriter(tmpFile);
            try {
                writer.write("Very simple text file");
            }
            finally {
                writer.close();
            }
            
            inputFilePath = tmpFile.getAbsolutePath();
        }        
        FrontendLauncher testDriver = new FrontendLauncher();
        String taskId = testDriver.createNewTask(inputFilePath,
                                                 this.wordcountJarPath);
        testDriver.waitForTaskCompletion(taskId);
        testDriver.getResults(taskId); // Just verifying results can be accessed
    }
    
    public void testParallelTasks() throws IOException, TestException {
        final String inputFilePath;
        final int taskCount = 4;        
        {
            File tmpFile = File.createTempFile("webui-wordcount", ".tmp");
            tmpFile.deleteOnExit();
            
            PrintWriter writer = new PrintWriter(tmpFile);
            try {
                writer.write("Very simple text file");
            }
            finally {
                writer.close();
            }
            
            inputFilePath = tmpFile.getAbsolutePath();
        }        
        FrontendLauncher testDriver = new FrontendLauncher();        
        String[] taskIds = new String[taskCount];
        for(int i = 0; i < taskCount; ++i) {
            taskIds[i] = testDriver.createNewTask(inputFilePath,
                                                  this.wordcountJarPath,
                                                  false);
        }        
        for(int i = 0; i < taskCount; ++i) {
            assertEquals(TaskStatus.Created,
                         testDriver.getTaskStatus(taskIds[i]),
                         "Veryfing task is in created state. TaskId: "
                                + taskIds[i]);
        }        
        for(int i = 0; i < taskCount; ++i) {
            testDriver.launchTask(taskIds[i]);
        }        
        for(int i = 0; i < taskCount; ++i) {
            TaskStatus jobStatus = testDriver.getTaskStatus(taskIds[i]);
            assertTrue(jobStatus == TaskStatus.Completed
                    || jobStatus == TaskStatus.Running,
                       "Verifying task is in running or completed state."
                    + " TaskId: " + taskIds[i]);
        }        
        for(int i = 0; i < taskCount; ++i) {
            testDriver.waitForTaskCompletion(taskIds[i]);
        }        
        for(int i = 0; i < taskCount; ++i) {
            TaskStatus jobStatus = testDriver.getTaskStatus(taskIds[i]);
            assertEquals(jobStatus,
                         TaskStatus.Completed,
                         "Verifying task is in completed state."
                    + " TaskId: " + taskIds[i]);
        }        
        for(int i = 0; i < taskCount; ++i) {
            // Just verifying results can be accessed
            testDriver.getResults(taskIds[i]);
        }
    }
}
