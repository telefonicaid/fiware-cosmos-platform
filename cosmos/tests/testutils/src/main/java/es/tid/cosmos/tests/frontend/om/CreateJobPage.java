package es.tid.cosmos.tests.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class CreateJobPage {
    // HTML classes and ids
    public static final String INPUT_FILE_HTML_ID = "id_file";
    public static final String JAR_FILE_HTML_ID = "id_jar";
    public static final String NAME_FILE_HTML_ID = "id_name";
    private WebDriver driver;
    private final String selectInputUrl = "TODO"; // TODO    

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.selectInputUrl);
    }

    public CreateJobPage(WebDriver driver) {
        this.driver = driver;
    }

    public void setInputFile(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
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

    public void create() {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_FILE_HTML_ID));
        inputElement.submit();
    }
}
