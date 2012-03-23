package es.tip.bdp.frontend_om;

import java.net.MalformedURLException;
import java.net.URL;

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
        goHome();
        WebElement row = getTaskRow(taskId);

        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public String getTaskLink(String taskId) throws MalformedURLException {
        goHome();
        WebElement row = getTaskRow(taskId);
        
        URL baseUrl = new URL(HOME_URL);
        String verbatimUrl = row.findElement(By.name(RESULT_LINK_CLASS))
                                    .getAttribute("href");
        return new URL(baseUrl, verbatimUrl).toString();
    }
    
    private WebElement getTaskRow(String taskId) {
        WebElement table = this.driver.findElement(By.id(TASK_STATUS_TABLE_ID));
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public void launchJob(String taskID) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public SelectNamePage goToCreateNewJob() {
        goHome();
        WebElement createJobElement = this.driver.findElement(By.id(CREATE_JOB_ID));
        createJobElement.click();
        return new SelectNamePage(this.driver);
    }
    
    public WebDriver getDriver() {
        return this.driver;
    }
    
}
