package es.tip.bdp.frontend_om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import org.testng.Assert;

public class SelectNamePage {
    private static final String NAME_ID = "SomeFakeId0";
    private static final String SELECT_NAME_URL = "";
    private WebDriver _driver;
    
    private void assertCorrectUrl() {
        Assert.assertEquals(_driver.getCurrentUrl(), SELECT_NAME_URL);
    }
    
    public SelectNamePage(WebDriver driver) {
        _driver = driver;
        assertCorrectUrl();
    }
    
    public void setName(String taskName) {
        assertCorrectUrl();
        WebElement nameElement = _driver.findElement(By.id(NAME_ID));
        nameElement.sendKeys(taskName);
    }
    
    public SelectJarPage submitNameForm() {
        assertCorrectUrl();
        WebElement nameElement = _driver.findElement(By.id(NAME_ID));
        nameElement.submit();
        return new SelectJarPage(_driver);
    }
}
