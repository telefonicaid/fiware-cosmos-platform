package es.tid.bdp.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class SelectJarPage {
    public static final String INPUT_JAR_HTML_ID = "jar-file";
    public static final String JAR_RESTRICTIONS_ID = "jar-restrictions";
    public static final String SAMPLE_JAR_LINK_ID = "sample-jar-link";
    
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
        
        return new SelectInputPage(this.driver);
    }
}
