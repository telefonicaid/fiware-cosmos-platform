package es.tid.bdp.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class SelectInputPage {
    public static final String INPUT_FILE_HTML_ID = "id_file";
    
    private WebDriver driver;
    private final String selectInputUrl;    
    
    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.selectInputUrl);
    }
    
    public SelectInputPage(WebDriver driver) {
        this.driver = driver;
        this.selectInputUrl = driver.getCurrentUrl();
    }
    
    public void setInputFile(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }
    
    public void submitInputFileForm() {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_FILE_HTML_ID));
        inputElement.submit();
    }
}
