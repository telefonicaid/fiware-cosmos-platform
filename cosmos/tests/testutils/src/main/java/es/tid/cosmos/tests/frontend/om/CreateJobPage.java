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

import java.util.Set;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotSame;

/**
 *
 * @author ximo
 */
public class CreateJobPage {
    // HTML classes and ids
    public static final String CANCEL_LINK_CLASS = "cos-cancel";
    public static final String NEXT_NAME = "next";
    public static final String BACK_NAME = "back";
    public static final String JAR_FILE_HTML_ID = "id_jar_path";
    public static final String NAME_FILE_HTML_ID = "id_name";
    public static final String SAMPLE_JAR_LINK_ID = "sample-jar-link";
    public static final String JAR_RESTRICTIONS_ID = "jar-restrictions";
    private final String createJobUrl;
    private final String mainWindow;
    private WebDriver driver;

    public String getJarRestrictions() {
        assertCorrectUrl();
        this.driver.findElement(By.id(JAR_RESTRICTIONS_ID)).click();
        this.switchToPopup();
        String source = this.driver.getPageSource();
        this.driver.get(this.createJobUrl);
        return source;

    }

    private void switchToPopup() {
        Set<String> handles = this.driver.getWindowHandles();
        assertEquals(handles.size(), 2);
        for (String handle : handles) {
            if (!handle.equals(this.mainWindow)) {
                this.driver.switchTo().window(handle);
                break;
            }
        }
    }

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.createJobUrl);
    }

    public CreateJobPage(WebDriver driver) {
        this.driver = driver;
        this.createJobUrl = this.driver.getCurrentUrl();
        this.mainWindow = this.driver.getWindowHandle();
    }

    public void setName(String name) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(NAME_FILE_HTML_ID));
        inputElement.sendKeys(name);
    }

    public void setInputJar(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(JAR_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }

    public SetParametersPage next() {
        assertCorrectUrl();
        WebElement nextElement = this.driver.findElement(
                By.name(NEXT_NAME));
        nextElement.click();
        return new SetParametersPage(this.driver);
    }

    public CreateJobPage cancel() {
        assertCorrectUrl();
        WebElement cancelLink = this.driver.findElement(
                By.className(CANCEL_LINK_CLASS));
        cancelLink.click();
        return new CreateJobPage(this.driver);
    }

    public String getErrors() {
        assertCorrectUrl();
        return this.driver.findElement(By.className("errorlist")).getText();
    }
}
