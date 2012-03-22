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
    private final String _selectJarUrl;
    public static final String INPUT_JAR_HTML_ID = "jar-file";
    public static final String JAR_RESTRICTIONS_ID = "jar-restrictions";
    public static final String SAMPLE_JAR_LINK_ID = "sample-jar-link";
    
    private void assertCorrectUrl() {
        Assert.assertEquals(_driver.getCurrentUrl(), _selectJarUrl);
    }
    
    public SelectJarPage(WebDriver driver) {
        _driver = driver;
        _selectJarUrl = driver.getCurrentUrl();
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
