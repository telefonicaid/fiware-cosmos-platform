package es.tip.bdp.frontend.om;

import java.net.MalformedURLException;
import java.net.URL;
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
    public static final String HOME_URL = "http://pstools/dev/bdp/";
    public static final String CREATE_JOB_ID = "create-job";
    public static final String TASK_STATUS_TABLE_ID = "results-table";
    public static final String RESULT_LINK_CLASS = "result-link";
    public static final String RESULT_NAME_CLASS = "job-name";
    public static final String RESULT_STATUS_CLASS = "job-status";
    private WebDriver driver;

    public FrontEnd() {
        this.driver = new HtmlUnitDriver();
    }

    public void goHome() {
        this.driver.get(HOME_URL);
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
        return this.getTaskRow(taskId).findElement(By.name(RESULT_LINK_CLASS));
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

        throw new IllegalArgumentException("No row found with provided taskId: "
                + taskId);
    }

    public void launchJob(String taskId) {
        this.goHome();
        if (TaskStatus.Created != this.getTaskStatus(taskId)) {
            throw new IllegalArgumentException(
                    "Task is not in the created state. taskId: " + taskId);
        }
        this.getTaskLink(taskId).click();
    }
    
    public ResultsPage goToResultsPage(String taskId)
            throws MalformedURLException {
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
