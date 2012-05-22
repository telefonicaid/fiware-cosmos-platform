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
public class UploadJarPage {
    // HTML classes and ids
    public static final String JAR_FILE_HTML_ID = "id_hdfs_file";
    public static final String DATA_NAME_HTML_NAME = "dest";
    public static final String ERROR_HTML_CLASS = "jframe-error-popup";
    public static final String FORMERROR_HTML_CLASS = "errorlist";
    private static final String UTF8 = "UTF-8";
    private WebDriver driver;
    private final String uploadJarUrl;

    private void assertCorrectUrl() {
        try {
            assertEquals(
                    URLDecoder.decode(this.driver.getCurrentUrl(), UTF8),
                    this.uploadJarUrl);
        } catch (UnsupportedEncodingException ex) {
            throw new TestException("Test bug", ex);
        }
    }

    public UploadJarPage(WebDriver driver) {
        this.driver = driver;
        try {
            this.uploadJarUrl = URLDecoder.decode(
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

    public void setJarFile(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(JAR_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }

    public void submitForm() {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(JAR_FILE_HTML_ID));
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
