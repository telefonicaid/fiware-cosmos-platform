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

/**
 *
 * @author ximo
 */
public class UploadPage {
    public static final String UPLOAD_DATA_CLASS = "cos-upload_dataset";
    public static final String UPLOAD_JAR_CLASS = "cos-upload_jar";

    private final String uploadPageUrl;
    private final String mainWindow;
    private WebDriver driver;

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.uploadPageUrl);
    }

    public UploadPage(WebDriver driver) {
        this.driver = driver;
        this.mainWindow = this.driver.getWindowHandle();
        this.uploadPageUrl = driver.getCurrentUrl();
    }

    public UploadJarPage goToUploadJar() {
        this.assertCorrectUrl();
        WebElement createJobElement = this.driver.findElement(
                By.className(UPLOAD_JAR_CLASS));
        createJobElement.click();
        this.switchToPopup();
        return new UploadJarPage(this.driver);
    }

    private void switchToPopup() {
        Set<String> handles = this.driver.getWindowHandles();
        assertEquals(handles.size(), 2);
        for(String handle : handles) {
            if(!handle.equals(this.mainWindow)) {
                this.driver.switchTo().window(handle);
                break;
            }
        }
    }

    public UploadDataPage goToUploadData() {
        this.assertCorrectUrl();
        WebElement createJobElement = this.driver.findElement(
                By.className(UPLOAD_DATA_CLASS));
        createJobElement.click();
        this.switchToPopup();
        return new UploadDataPage(this.driver);
    }
}
