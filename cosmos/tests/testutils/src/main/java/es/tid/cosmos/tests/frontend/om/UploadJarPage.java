package es.tid.cosmos.tests.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class UploadJarPage {
    // HTML classes and ids
    public static final String JAR_FILE_HTML_ID = "id_jar";
    private WebDriver driver;
    private final String uploadJarUrl = "TODO"; // TODO

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.uploadJarUrl);
    }

    public UploadJarPage(WebDriver driver) {
        this.driver = driver;
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
}
