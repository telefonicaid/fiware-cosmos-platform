package es.tid.cosmos.tests.frontend.om;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.htmlunit.HtmlUnitDriver;
import static org.testng.Assert.assertTrue;
import static org.testng.Assert.fail;

import es.tid.cosmos.tests.tasks.Environment;
import static es.tid.cosmos.tests.tasks.EnvironmentSetting.CosmosRelativeUrl;
import static es.tid.cosmos.tests.tasks.EnvironmentSetting.FrontendUrl;
import es.tid.cosmos.tests.tasks.TaskStatus;
import es.tid.cosmos.tests.tasks.TestException;

/**
 *
 * @author ximo
 */
public class FrontEnd {
    // HTML classes and ids
    public static final String CREATE_JOB_ID = "create-job";
    public static final String TASK_STATUS_TABLE_ID = "jobs-table";
    public static final String JOB_ACTION_CLASS = "jobaction";
    public static final String RESULT_ACTION_CLASS = "results";
    public static final String RUN_ACTION_CLASS = "run";
    public static final String UPLOAD_DATA_ACTION_CLASS = "upload-data";
    public static final String RESULT_NAME_CLASS = "result-name";
    public static final String RESULT_STATUS_CLASS = "result-status";
    public static final String USERNAME_INPUT_NAME = "username";
    public static final String PASSWORD_INPUT_NAME = "password";
    // Default login info
    private static final String DEFAULT_USER = "test";
    private static final String DEFAULT_PASSWRD = "cosmostest";
    private WebDriver driver;
    private final String username;
    private final String password;
    private final String baseUrl;
    private final String cosmosUrl;
    private final Environment environment;

    public FrontEnd(Environment env) {
        this(env, DEFAULT_USER, DEFAULT_PASSWRD);
    }

    public FrontEnd(Environment env, String username, String password) {
        this.driver = new HtmlUnitDriver();
        this.username = username;
        this.password = password;
        this.environment = env;
        this.baseUrl = this.environment.getProperty(FrontendUrl);
        try {
            URL base = new URL(baseUrl);
            this.cosmosUrl = new URL(
                    base,
                    this.environment.getProperty(CosmosRelativeUrl)).toString();
        } catch (MalformedURLException ex) {
            throw new TestException("[Test bug] Malformed URL for frontend. " + ex.toString());
        }
    }

    public Environment getEnvironment() {
        return this.environment;
    }

    public URL resolveURL(String verbatimUrl) throws MalformedURLException {
        return new URL(new URL(this.driver.getCurrentUrl()), verbatimUrl);
    }

    public void gotoCosmosHome() {
        this.driver.get(this.cosmosUrl);
        this.login(this.username, this.password);
    }

    private boolean needLogin() {
        return this.driver.getCurrentUrl().contains("login");
    }

    private void login(String user, String pass) {
        if (!this.needLogin()) {
            return;
        }

        WebElement userElement = this.driver.findElement(
                By.name(USERNAME_INPUT_NAME));
        userElement.sendKeys(user);
        this.driver.findElement(By.name(PASSWORD_INPUT_NAME)).sendKeys(pass);

        userElement.submit();
        if (this.needLogin()) {
            fail("[Test bug] Bad user/password. User: " + user + ". Password: " + pass);
        }
    }

    public boolean taskExists(String taskId) {
        this.gotoCosmosHome();
        return this.getTaskRow(taskId) != null;

    }

    public TaskStatus getTaskStatus(String taskId) {
        this.gotoCosmosHome();
        WebElement row = this.getTaskRow(taskId);

        String statusText = row.findElement(
                By.className(RESULT_STATUS_CLASS)).getText();
        if (statusText.equals("Running")) {
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

    private WebElement getTaskLink(String taskId, String expectedClass) {
        WebElement taskLink = this.getTaskRow(taskId).findElement(By.className(JOB_ACTION_CLASS));

        boolean isExpectedLink = false;
        for (String elementClass : taskLink.getAttribute("class").split("\\s")) {
            if (elementClass.equals(expectedClass)) {
                isExpectedLink = true;
                break;
            }
        }

        assertTrue(isExpectedLink, "Verifying link contains class: " + expectedClass);
        return taskLink;
    }

    /**
     * Returns the first row that has the taskId
     */
    private WebElement getTaskRow(String taskId) {
        WebElement table = this.driver.findElement(By.id(TASK_STATUS_TABLE_ID));
        List<WebElement> rows = table.findElement(By.tagName("tbody")).findElements(By.tagName("tr"));
        for (WebElement row : rows) {
            WebElement nameElement = row.findElement(
                    By.className(RESULT_NAME_CLASS));
            if (taskId.equals(nameElement.getText())) {
                return row;
            }
        }

        return null;
    }

    public ResultsPage goToResultsPage(String taskId) {
        this.gotoCosmosHome();
        TaskStatus status = this.getTaskStatus(taskId);
        if (TaskStatus.Completed != status) {
            throw new IllegalArgumentException(
                    "Task is not in the completed state.\n"
                    + "taskId: " + taskId + "\n"
                    + "status: " + status + "\n");
        }

        this.getTaskLink(taskId, RESULT_ACTION_CLASS).click();
        return new ResultsPage(this.driver);
    }

    public ResultsPage goToFailurePage(String taskId) {
        this.gotoCosmosHome();
        TaskStatus status = this.getTaskStatus(taskId);
        if (TaskStatus.Error != status) {
            throw new IllegalArgumentException(
                    "Task is not in the error state.\n"
                    + "taskId: " + taskId + "\n"
                    + "status: " + status + "\n");
        }

        this.getTaskLink(taskId, RESULT_ACTION_CLASS).click();
        return new ResultsPage(this.driver);
    }

    public CreateJobPage goToCreateNewJob() {
        this.gotoCosmosHome();
        WebElement createJobElement = this.driver.findElement(
                By.id(CREATE_JOB_ID));
        createJobElement.click();
        return new CreateJobPage(this.driver);
    }

    public WebDriver getDriver() {
        return this.driver;
    }
}
