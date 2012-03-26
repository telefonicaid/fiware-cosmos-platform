package es.tid.bdp.frontend.om;

import java.net.MalformedURLException;
import java.util.List;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.htmlunit.HtmlUnitDriver;

import es.tid.bdp.joblaunchers.TaskStatus;

/**
 *
 * @author ximo
 */
public class FrontEnd {
    // TODO: Change Home URL to a config parameter in future iterations
    public static final String HOME_URL = "http://pshdp01:8000/";
    public static final String CREATE_JOB_ID = "create-job";
    public static final String TASK_STATUS_TABLE_ID = "jobs-table";
    public static final String RESULT_LINK_CLASS = "result-link";
    public static final String RESULT_NAME_CLASS = "result-name";
    public static final String RESULT_STATUS_CLASS = "result-status";
    public static final String USERNAME_INPUT_ID = "id_username";
    public static final String PASSWORD_INPUT_ID = "id_password";
    
    private static final String DEFAULT_USER = "admin";
    private static final String DEFAULT_PASSWRD = "admin";
    
    private WebDriver driver;
    private String username;
    private String password;

    public FrontEnd() {
        this(DEFAULT_USER, DEFAULT_PASSWRD);
    }
    
    public FrontEnd(String username, String password) {
        this.driver = new HtmlUnitDriver();
        this.username = username;
        this.password = password;
    }

    public void goHome() {
        this.driver.get(HOME_URL);
        if(this.driver.getCurrentUrl().contains("login")) {
            this.login("admin", "admin");
        }
    }
    
    private void login(String user, String pass) {
        WebElement userElement = this.driver
                .findElement(By.id(USERNAME_INPUT_ID));
        userElement.sendKeys(user);
        this.driver.findElement(By.id(PASSWORD_INPUT_ID)).sendKeys(pass);
        
        userElement.submit();
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
        if (statusText.equals("Created")) {
            return TaskStatus.Created;
        } else if (statusText.equals("Running")) {
            return TaskStatus.Running;
        } else if (statusText.equals("Succeeded")) {
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
                By.className(RESULT_LINK_CLASS));
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
