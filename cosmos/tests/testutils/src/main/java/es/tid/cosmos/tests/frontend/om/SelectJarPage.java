package es.tid.cosmos.tests.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotSame;

/**
 *
 * @author ximo
 */
public class SelectJarPage {
    // HTML classes and ids
    public static final String INPUT_JAR_HTML_ID = "id_file";
    
    private WebDriver driver;
    private final String selectJarUrl;    
    
    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.selectJarUrl);
    }
    
    public SelectJarPage(WebDriver driver) {
        this.driver = driver;
        this.selectJarUrl = driver.getCurrentUrl();
    }
    
    public void setInputJar(String filePath) {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_JAR_HTML_ID));
        inputElement.sendKeys(filePath);
    }
    
    public SelectInputPage submitJarFileForm() {
        assertCorrectUrl();
        WebElement inputElement = this.driver.findElement(
                By.id(INPUT_JAR_HTML_ID));
        inputElement.submit();
        assertNotSame(this.driver.getCurrentUrl(), this.selectJarUrl,
                      "Verifying URL has changed");
        
        return new SelectInputPage(this.driver);
    }
}
