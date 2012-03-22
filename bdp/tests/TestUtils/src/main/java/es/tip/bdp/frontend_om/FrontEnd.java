package es.tip.bdp.frontend_om;

import es.tid.bdp.joblaunchers.TaskStatus;
import java.net.MalformedURLException;
import java.net.URL;
import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.htmlunit.HtmlUnitDriver;

/**
 *
 * @author ximo
 */
public class FrontEnd {
    public static final String HOME_URL = "http://pstools/dev/bdp/";
    private static final String CREATE_JOB_ID = "SomeFakeId0";
    private static final String TASK_STATUS_TABLE_ID = "SomeFakeId0";
    
    private WebDriver _driver;
    
    public FrontEnd() {
        _driver = new HtmlUnitDriver(); 
    }
    
    public void goHome() {
        _driver.get(HOME_URL);
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
        String verbatimUrl = row.findElement(By.name("link"))
                                    .getAttribute("href");
        return new URL(baseUrl, verbatimUrl).toString();
    }
    
    private WebElement getTaskRow(String taskId) {
        WebElement table = _driver.findElement(By.id(TASK_STATUS_TABLE_ID));
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public void launchJob(String taskID) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public SelectNamePage goToCreateNewJob() {
        goHome();
        WebElement createJobElement = _driver.findElement(By.id(CREATE_JOB_ID));
        createJobElement.click();
        return new SelectNamePage(_driver);
    }
    
    public WebDriver getDriver() {
        return _driver;
    }
    
}
