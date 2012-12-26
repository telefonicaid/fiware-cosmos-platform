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

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

import static es.tid.cosmos.tests.frontend.om.CreateJobPage.BACK_NAME;
import static es.tid.cosmos.tests.frontend.om.CreateJobPage.CANCEL_LINK_CLASS;
import static es.tid.cosmos.tests.frontend.om.CreateJobPage.NEXT_NAME;

/**
 *
 * @author ximo
 */
public class ReviewSettings {

    private WebDriver driver;
    private final String reviewSettingsUrl;

    public ReviewSettings(WebDriver driver) {
        this.driver = driver;
        this.reviewSettingsUrl = driver.getCurrentUrl();
    }

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.reviewSettingsUrl);
    }

    public void runJob() {
        assertCorrectUrl();
        WebElement nextElement = this.driver.findElement(
                By.name(NEXT_NAME));
        nextElement.click();
    }

    public SetParametersPage back() {
        assertCorrectUrl();
        WebElement backElement = this.driver.findElement(
                By.name(BACK_NAME));
        backElement.click();
        return new SetParametersPage(this.driver);
    }

    public CreateJobPage cancel() {
        assertCorrectUrl();
        WebElement cancelLink = this.driver.findElement(
                By.className(CANCEL_LINK_CLASS));
        cancelLink.click();
        return new CreateJobPage(this.driver);
    }
}
