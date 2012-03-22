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
    private static final String SELECT_INPUT_URL = "";
    private static final String INPUT_FILE_HTML_ID = "SomeFakeId";
    
    private void assertCorrectUrl() {
        Assert.assertEquals(_driver.getCurrentUrl(), SELECT_INPUT_URL);
    }
    
    public SelectInputPage(WebDriver driver) {
        _driver = driver;
        assertCorrectUrl();
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
