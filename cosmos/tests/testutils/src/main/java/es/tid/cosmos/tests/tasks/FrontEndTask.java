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

package es.tid.cosmos.tests.tasks;

import java.io.File;
import java.util.List;
import java.util.Map;
import java.util.UUID;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNotSame;

import es.tid.cosmos.tests.environment.Environment;
import es.tid.cosmos.tests.frontend.om.*;

/**
 *
 * @author ximo
 */
public class FrontEndTask extends Task {
    private static final String FILE_BROWSER_URL = "filebrowser/view/";
    private boolean isRun;
    private final FrontEnd frontend;
    private final String taskId;
    private final String jarHdfsPath;

    public static String getOutputHdfsPath(String user, String outputDirName) {
        return ("/user/" + user + "/" + outputDirName);
    }

    public static String getJarHdfsPath(String user, String jarFileName) {
        return ("/user/" + user + "/jars/" + jarFileName);
    }

    public static String getJarHdfsPath(String user) {
        return getJarHdfsPath(user, "");
    }

    public static String getDataHdfsPath(String user, String dataFileName) {
        return ("/user/" + user + "/datasets/" + dataFileName);
    }

    public static String getDataHdfsPath(String user) {
        return getDataHdfsPath(user, "");
    }

    public static boolean jarExists(FrontEnd frontend, String jarName) {
        return fileExists(frontend, getJarHdfsPath(frontend.getUsername()),
                          jarName);
    }

    public static boolean dataSetExists(FrontEnd frontend,  String dataSetName) {
        return fileExists(frontend, getDataHdfsPath(frontend.getUsername()),
                          dataSetName);
    }

    private static boolean fileExists(FrontEnd frontend, String hdfsPath,
                                      String fileName) {
        frontend.gotoCosmosHome();
        WebDriver driver = frontend.getDriver();
        driver.get(frontend.getBaseUrl() + "/" + FILE_BROWSER_URL + hdfsPath);
        List<WebElement> files = driver.findElements(By.className("fb-file"));
        for (WebElement file : files) {
            if (file.getText().equals(fileName)) {
                return true;
            }
        }
        return false;
    }

    public static void uploadJar(FrontEnd frontend, String filePath) {
        UploadPage uploadPage = frontend.goToUpload();
        UploadJarPage uploadJarPage = uploadPage.goToUploadJar();
        uploadJarPage.setName(new File(filePath).getName());
        uploadJarPage.setJarFile(filePath);
        uploadJarPage.submitForm();
    }

    public static void uploadData(FrontEnd frontend, String filePath) {
        UploadPage uploadPage = frontend.goToUpload();
        UploadDataPage uploadDataPage = uploadPage.goToUploadData();
        uploadDataPage.setName(new File(filePath).getName());
        uploadDataPage.setDataFile(filePath);
        uploadDataPage.submitForm();
    }

    public FrontEndTask(FrontEnd frontend,
                        String jarHdfsPath,
                        String taskId) {
        this.frontend = frontend;
        this.taskId = taskId;
        this.jarHdfsPath = jarHdfsPath;
        this.isRun = false;
    }

    public FrontEndTask(FrontEnd frontend,
                        String jarPath) {
        this(frontend, jarPath, UUID.randomUUID().toString());
    }

    public FrontEndTask(Environment env, String jarPath) {
        this(new FrontEnd(env), jarPath);
    }

    public FrontEndTask(Environment env, String jarPath,
                        String taskId) {
        this(new FrontEnd(env), jarPath, taskId);
    }

    public static FrontEndTask createFromExistingTaskId(Environment env,
                                                        String taskId) {
        return new FrontEndTask(env, null, taskId);
    }

    @Override
    public void run() {
        assertFalse(this.isRun, "Veryfing run hasn't been called previously."
                + " If this fails, it is a test bug.");

        CreateJobPage createJobPage = this.frontend.goToCreateNewJob();
        createJobPage.setName(this.taskId);
        createJobPage.setInputJar(this.jarHdfsPath);
        SetParametersPage setParamsPage = createJobPage.next();
        if (this.dataSet != null) {
            setParamsPage.setDatasetPath(this.dataSet);
        }

        for(String key : this.parameters.keySet()) {
            setParamsPage.setParameter(key, this.parameters.get(key));
        }

        final String prevUrl = this.frontend.getDriver().getCurrentUrl();
        setParamsPage.next().runJob();
        assertNotSame(prevUrl, this.frontend.getDriver().getCurrentUrl(),
                      "Veryfing job submission was successfull by looking at"
                      + "the URL");
        this.isRun = true;
    }

    @Override
    public TaskStatus getStatus() {
        if (!this.isRun) {
            return TaskStatus.Created;
        }
        return this.frontend.getTaskStatus(taskId);
    }

    @Override
    public List<Map<String, String>> getResults() {
        ResultsPage resultsPage = this.frontend.goToResultsPage(this.taskId);
        return resultsPage.getResults();
    }

    @Override
    public String toString() {
        return "[FrontEndTask] Id = " + taskId;
    }
}
