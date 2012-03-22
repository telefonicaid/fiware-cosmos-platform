package es.tip.bdp.frontend_om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.testng.Assert;

/**
 *
 * @author ximo
 */
public class SelectJarPage {
    private WebDriver _driver;
    private static final String SELECT_JAR_URL = "";
    private static final String INPUT_JAR_HTML_ID = "SomeFakeId";
    
    private void assertCorrectUrl() {
        Assert.assertEquals(_driver.getCurrentUrl(), SELECT_JAR_URL);
    }
    
    public SelectJarPage(WebDriver driver) {
        _driver = driver;
        assertCorrectUrl();
    }
    
    public void setInputJar(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = _driver.findElement(By.id(INPUT_JAR_HTML_ID));
        inputElement.sendKeys(filePath);
    }
    
    public SelectInputPage submitJarFileForm() {
        assertCorrectUrl();
        WebElement inputElement = _driver.findElement(By.id(INPUT_JAR_HTML_ID));
        inputElement.submit();
        
        return new SelectInputPage(_driver);
    }
}
