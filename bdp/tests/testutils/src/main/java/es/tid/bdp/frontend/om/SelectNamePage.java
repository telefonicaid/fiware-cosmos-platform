package es.tid.bdp.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

public class SelectNamePage {
    public static final String NAME_ID = "id_name";    
    public static final String JAR_RESTRICTIONS_ID = "jar-restrictions";
    public static final String SAMPLE_JAR_LINK_ID = "sample-jar-link";
    public static final String SAMPLE_JAR_SOURCES_LINK_ID = "sample-jar-sources";
    
    private final String selectNameUrl;
    private WebDriver driver;
    
    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.selectNameUrl);
    }
    
    public SelectNamePage(WebDriver driver) {
        this.driver = driver;
        this.selectNameUrl = driver.getCurrentUrl();
    }
    
    public void setName(String taskName) {
        assertCorrectUrl();
        WebElement nameElement = this.driver.findElement(By.id(NAME_ID));
        nameElement.sendKeys(taskName);
    }
    
    public SelectJarPage submitNameForm() {
        assertCorrectUrl();
        WebElement nameElement = this.driver.findElement(By.id(NAME_ID));
        nameElement.submit();
        return new SelectJarPage(this.driver);
    }
    
    public String getJarRestrictions() {
        assertCorrectUrl();
        this.driver.findElement(By.id(JAR_RESTRICTIONS_ID)).click();
        String source = this.driver.getPageSource();
        this.driver.get(selectNameUrl);
        return source;
    }
}
