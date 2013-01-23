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

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

import es.tid.cosmos.tests.tasks.TestException;

/**
 *
 * @author ximo
 */
public class UploadDataPage {
    // HTML classes and ids
    public static final String DATA_FILE_HTML_ID = "id_hdfs_file";
    public static final String DATA_NAME_HTML_NAME = "dest";
    public static final String ERROR_HTML_CLASS = "jframe-error-popup";
    public static final String FORMERROR_HTML_CLASS = "errorlist";
    private static final String UTF8 = "UTF-8";
    private WebDriver driver;
    private final String uploadDataUrl;

    private void assertCorrectUrl() {
        try {
            assertEquals(
                    URLDecoder.decode(this.driver.getCurrentUrl(), UTF8),
                    this.uploadDataUrl);
        } catch (UnsupportedEncodingException ex) {
            throw new TestException("Test bug", ex);
        }
    }

    public UploadDataPage(WebDriver driver) {
        this.driver = driver;
        try {
            this.uploadDataUrl = URLDecoder.decode(
                    this.driver.getCurrentUrl(), UTF8);
        } catch (UnsupportedEncodingException ex) {
            throw new TestException("Test bug", ex);
        }
    }

    public void setName(String fileName) {
        assertCorrectUrl();
        WebElement nameElement = this.driver.findElement(
                By.name(DATA_NAME_HTML_NAME));
        nameElement.sendKeys(fileName);
    }

    public void setDataFile(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(DATA_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }

    public void submitForm() {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(DATA_FILE_HTML_ID));
        inputElement.submit();
    }

    public String getErrorText() {
        WebElement errorElement = this.driver.findElement(
                By.className(ERROR_HTML_CLASS));
        return errorElement.getText();
    }

    public String getFormErrorText() {
        WebElement errorElement = this.driver.findElement(
                By.className(FORMERROR_HTML_CLASS));
        return errorElement.getText();
    }
}
