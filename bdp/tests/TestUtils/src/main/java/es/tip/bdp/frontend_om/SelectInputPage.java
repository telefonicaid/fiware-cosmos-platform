package es.tip.bdp.frontend_om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.testng.Assert;

/**
 *
 * @author ximo
 */
public class SelectInputPage {
    private WebDriver _driver;
    private final String _selectInputUrl;
    public static final String INPUT_FILE_HTML_ID = "input-file";
    
    private void assertCorrectUrl() {
        Assert.assertEquals(_driver.getCurrentUrl(), _selectInputUrl);
    }
    
    public SelectInputPage(WebDriver driver) {
        _driver = driver;
        _selectInputUrl = driver.getCurrentUrl();
    }
    
    public void setInputFile(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = _driver.findElement(By.id(INPUT_FILE_HTML_ID));
        inputElement.sendKeys(filePath);
    }
    
    public void submitInputFileForm() {
        assertCorrectUrl();
        WebElement inputElement = _driver.findElement(By.id(INPUT_FILE_HTML_ID));
        inputElement.submit();
    }
}
