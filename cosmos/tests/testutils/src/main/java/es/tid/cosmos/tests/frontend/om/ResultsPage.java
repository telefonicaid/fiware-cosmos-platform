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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.openqa.selenium.support.ui.Select;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class ResultsPage {
    // HTML classes and ids
    public static final String RESULT_TABLE_ID = "job-results-table";
    public static final String RESULT_CELL_ID = "job-result-value";
    public static final String SELECT_PRIMARYKEY_ID = "select-pk";
    private WebDriver driver;
    private final String resultPageUrl;

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.resultPageUrl);
    }

    public ResultsPage(WebDriver driver) {
        this.driver = driver;
        this.resultPageUrl = driver.getCurrentUrl();
    }

    public List<Map<String, String>> getResults() {
        assertCorrectUrl();
        if (this.driver.getPageSource().contains("There were no results")) {
            return new ArrayList<Map<String, String>>();
        }

        final WebElement resultTable = driver.findElement(
                By.id(RESULT_TABLE_ID));

        // Get keys
        final String primaryKey = new Select(driver.findElement(
                By.id(SELECT_PRIMARYKEY_ID))).getFirstSelectedOption().getText();
        final List<WebElement> keysHtml = resultTable.findElement(
                By.tagName("thead")).findElements(By.tagName("th"));
        List<String> keys = new ArrayList<String>(keysHtml.size());
        for (WebElement keyHtml : keysHtml) {
            final String key = keyHtml.getText();
            if (!"_id".equals(key)) {
                keys.add(keyHtml.getText());
            }
        }

        // Get results
        final List<WebElement> resultsHtml = resultTable.findElement(
                By.tagName("tbody")).findElements(By.tagName("tr"));
        List<Map<String, String>> results = new ArrayList<Map<String, String>>(
                resultsHtml.size());
        for (WebElement resultHtml : resultsHtml) {
            Map<String, String> result = new HashMap<String, String>();

            // Set primary key
            result.put(primaryKey,
                       resultHtml.findElement(By.tagName("th")).getText());

            // Set the rest of keys
            List<WebElement> cells = resultHtml.findElements(
                    By.className(RESULT_CELL_ID));
            assertEquals(keys.size(),
                         cells.size(),
                         "Verifying the number of cells and keys are equal."
                    + " URL: " + this.driver.getCurrentUrl());
            for (int index = 0; index < keys.size(); ++index) {
                result.put(keys.get(index), cells.get(index).getText());
            }

            // Add it to the return variable
            results.add(result);
        }
        return results;
    }
}
