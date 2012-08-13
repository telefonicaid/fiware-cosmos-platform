/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.tests.frontend.om;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;
import java.util.Set;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.htmlunit.HtmlUnitDriver;
import static org.testng.Assert.assertTrue;
import static org.testng.Assert.fail;

import es.tid.cosmos.tests.environment.Environment;
import es.tid.cosmos.tests.environment.EnvironmentSetting;
import static es.tid.cosmos.tests.environment.EnvironmentSetting.CosmosRelativeUrl;
import es.tid.cosmos.tests.tasks.TaskStatus;
import es.tid.cosmos.tests.tasks.TestException;

/**
 *
 * @author ximo
 */
public class FrontEnd {
    // HTML classes and ids
    public static final String UPLOAD_CLASS = "cos-upload_nav";
    public static final String CREATE_JOB_CLASS = "cos-define_job_nav";
    public static final String TASK_STATUS_TABLE_CLASS = "job-listing";
    public static final String JOB_ACTION_CLASS = "job-action";
    public static final String RESULT_ACTION_CLASS = "results";
    public static final String RESULT_NAME_CLASS = "job-name";
    public static final String RESULT_STATUS_CLASS = "job-status";
    public static final String USERNAME_INPUT_ID = "id_username";
    public static final String PASSWORD_INPUT_ID = "id_password";
    private static final String DEFAULT_HTTP_PORT = "80";
    private WebDriver driver;
    private final String username;
    private final String password;
    private final String baseUrl;
    private final String cosmosUrl;
    private final Environment environment;

    public FrontEnd(Environment env) {
        this(env, env.getProperty(EnvironmentSetting.DefaultUser),
             env.getProperty(EnvironmentSetting.DefaultPassword));
    }

    public FrontEnd(Environment env, String username, String password) {
        this.driver = new HtmlUnitDriver();
        this.username = username;
        this.password = password;
        this.environment = env;
        this.baseUrl = FrontEnd.getBaseUrl(this.environment);
        try {
            URL base = new URL(baseUrl);
            this.cosmosUrl = new URL(
                    base,
                    this.environment.getProperty(CosmosRelativeUrl)).toString();
        } catch (MalformedURLException ex) {
            throw new TestException("[Test bug] Malformed URL for frontend. " + ex.toString());
        }
    }

    public static String getBaseUrl(Environment env) {
        String frontendServer = env.getProperty(
                EnvironmentSetting.FrontendServer);
        String frontendPort = env.getProperty(
                EnvironmentSetting.FrontendHttpPort);
        if (frontendPort.equals(DEFAULT_HTTP_PORT)) {
            frontendPort = "";
        } else {
            frontendPort = ":" + frontendPort;
        }
        return "http://" + frontendServer + frontendPort;
    }

    public String getBaseUrl() {
        return this.baseUrl;
    }

    public Environment getEnvironment() {
        return this.environment;
    }

    public String getUsername() {
        return this.username;
    }

    public URL resolveURL(String verbatimUrl) throws MalformedURLException {
        return new URL(new URL(this.driver.getCurrentUrl()), verbatimUrl);
    }

    public void gotoHueHome() {
        this.driver.get(this.baseUrl);
        this.login(this.username, this.password);
    }

    public void gotoCosmosHome() {
        this.purgePopups();
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
                By.id(USERNAME_INPUT_ID));
        userElement.sendKeys(user);
        this.driver.findElement(By.id(PASSWORD_INPUT_ID)).sendKeys(pass);

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
        if (statusText.equals("running") || statusText.equals("submitted")) {
            return TaskStatus.Running;
        } else if (statusText.equals("successful")) {
            return TaskStatus.Completed;
        } else if (statusText.equals("failed")) {
            return TaskStatus.Error;
        } else {
            throw new IllegalStateException("Task does not have a known state."
                    + "Status: " + statusText);
        }
    }

    private WebElement getTaskLink(String taskId, String expectedClass) {
        WebElement taskLink = this.getTaskRow(taskId).findElement(
                By.className(JOB_ACTION_CLASS));

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
        WebElement table = this.driver.findElement(
                By.className(TASK_STATUS_TABLE_CLASS));
        List<WebElement> rows = table.findElement(
                By.tagName("tbody")).findElements(By.tagName("tr"));
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
                By.className(CREATE_JOB_CLASS));
        createJobElement.click();
        CreateJobPage createJobPage = new CreateJobPage(this.driver);
        createJobPage.cancel();
        return createJobPage;
    }

    public UploadPage goToUpload() {
        this.gotoCosmosHome();
        WebElement createJobElement = this.driver.findElement(
                By.className(UPLOAD_CLASS));
        createJobElement.click();
        return new UploadPage(this.driver);
    }

    public WebDriver getDriver() {
        return this.driver;
    }

    private void purgePopups() {
        for (Set<String> handles = this.driver.getWindowHandles();
                handles.size() > 1;
                handles = this.driver.getWindowHandles()) {
            this.driver.switchTo().window(handles.iterator().next()).close();
        }

        this.driver.switchTo().window(
                this.driver.getWindowHandles().iterator().next());
    }
}
