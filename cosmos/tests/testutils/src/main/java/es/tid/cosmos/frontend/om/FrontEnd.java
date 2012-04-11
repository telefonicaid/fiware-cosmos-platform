package es.tid.cosmos.frontend.om;

import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;
import java.util.Properties;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.htmlunit.HtmlUnitDriver;
import static org.testng.Assert.fail;

import es.tid.cosmos.joblaunchers.Environment;
import es.tid.cosmos.joblaunchers.TaskStatus;
import es.tid.cosmos.joblaunchers.TestException;

/**
 *
 * @author ximo
 */
public class FrontEnd {
    public static final String TEST_CONFIG_FILE = "/test.properties";
    public static final String PRODUCTION_CONFIG_FILE = "/production.properties";
    public static final String CREATE_JOB_ID = "create-job";
    public static final String TASK_STATUS_TABLE_ID = "jobs-table";
    public static final String JOB_ACTION_CLASS = "jobaction";
    public static final String RESULT_NAME_CLASS = "result-name";
    public static final String RESULT_STATUS_CLASS = "result-status";
    public static final String USERNAME_INPUT_ID = "id_username";
    public static final String PASSWORD_INPUT_ID = "id_password";
    
    private static final String DEFAULT_USER = "test";
    private static final String DEFAULT_PASSWRD = "cosmostest";
    
    private WebDriver driver;
    private final String username;
    private final String password;
    private final String homeUrl;
    private final Environment environment;

    public FrontEnd(Environment env) {
        this(DEFAULT_USER, DEFAULT_PASSWRD, env);
    }
    
    public FrontEnd(String username, String password, Environment env) {
        this.driver = new HtmlUnitDriver();
        this.username = username;
        this.password = password;
        this.environment = env;
        
        Properties props = new Properties();
        try {
            switch(env) {
                case Test:
                    props.load(FrontEnd.class.getResource(TEST_CONFIG_FILE).openStream());
                    break;
                case Production:
                    props.load(FrontEnd.class.getResource(PRODUCTION_CONFIG_FILE).openStream());
                    break;
                default:
                    fail("Unrecognized environment value: " + env.toString());
            }
        } catch (IOException ex) {
            fail("IOException while loading configuration: " + ex.toString());
        }
        this.homeUrl = props.getProperty("FRONTEND_URL");
    }
    
    public String getHomeUrl() {
        return this.homeUrl;
    }
    
    public Environment getEnvironment() {
        return this.environment;
    }
    
    public URL resolveURL(String verbatimUrl) throws MalformedURLException {
        return new URL(new URL(this.driver.getCurrentUrl()), verbatimUrl);
    }

    public void goHome() {
        this.driver.get(this.homeUrl);
        if (this.driver.getCurrentUrl().contains("login")) {
            try {
                this.login(this.username, this.password);
            } catch (TestException ex) {
                fail("Bad user/password. " +  ex.toString());
            }
        }
    }
    
    private void login(String user, String pass) throws TestException {
        WebElement userElement = this.driver.findElement(By.id(USERNAME_INPUT_ID));
        userElement.sendKeys(user);
        this.driver.findElement(By.id(PASSWORD_INPUT_ID)).sendKeys(pass);
        
        userElement.submit();
        if (this.driver.getCurrentUrl().contains("login")) {
            throw new TestException("Login was not successful");
        }
    }
    
    public boolean taskExists(String taskId) {
        this.goHome();
        return this.getTaskRow(taskId) != null;
        
    }

    public TaskStatus getTaskStatus(String taskId) {
        this.goHome();
        WebElement row = this.getTaskRow(taskId);

        String statusText = row.findElement(
                By.className(RESULT_STATUS_CLASS)).getText();
        if (statusText.equals("Configured")) {
            return TaskStatus.Created;
        } else if (statusText.equals("Running")) {
            return TaskStatus.Running;
        } else if (statusText.equals("Successful")) {
            return TaskStatus.Completed;
        } else if (statusText.equals("Failed")) {
            return TaskStatus.Error;
        } else {
            throw new IllegalStateException("Task does not have a known state."
                    + "Status: " + statusText);
        }
    }

    private WebElement getTaskLink(String taskId) {
        return this.getTaskRow(taskId).findElement(
                By.className(JOB_ACTION_CLASS));
    }

    /**
     * Returns the first row that has the taskId
     */
    private WebElement getTaskRow(String taskId) {
        WebElement table = this.driver.findElement(By.id(TASK_STATUS_TABLE_ID));
        List<WebElement> rows = table.findElement(By.tagName("tbody"))
                .findElements(By.tagName("tr"));
        for (WebElement row : rows) {
            WebElement nameElement = row.findElement(
                    By.className(RESULT_NAME_CLASS));
            if (taskId.equals(nameElement.getText())) {
                return row;
            }
        }

        return null;
    }

    public void launchJob(String taskId) {
        this.goHome();
        if (TaskStatus.Created != this.getTaskStatus(taskId)) {
            throw new IllegalArgumentException(
                    "Task is not in the created state. taskId: " + taskId);
        }
        this.getTaskLink(taskId).click();
    }
    
    public ResultsPage goToResultsPage(String taskId) {
        this.goHome();
        TaskStatus status = this.getTaskStatus(taskId);
        if (TaskStatus.Created == status
                || TaskStatus.Running == status) {
            throw new IllegalArgumentException(
                    "Task does not have a result yet.\n"
                    + "taskId: " + taskId + "\n"
                    + "status: " + status + "\n");
        }

        this.getTaskLink(taskId).click();
        return new ResultsPage(this.driver);
    }

    public SelectNamePage goToCreateNewJob() {
        this.goHome();
        WebElement createJobElement = this.driver
                .findElement(By.id(CREATE_JOB_ID));
        createJobElement.click();
        return new SelectNamePage(this.driver);
    }

    public WebDriver getDriver() {
        return this.driver;
    }
}
