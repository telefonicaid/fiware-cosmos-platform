package es.tid.cosmos.tests.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class UploadDataPage {
    // HTML classes and ids
    public static final String DATA_FILE_HTML_ID = "id_jar";
    private WebDriver driver;
    private final String uploadDataUrl = "TODO"; // TODO

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.uploadDataUrl);
    }

    public UploadDataPage(WebDriver driver) {
        this.driver = driver;
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
}
