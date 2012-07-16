
package es.tid.cosmos.tests.frontend.om;

import org.openqa.selenium.By;
import org.openqa.selenium.WebDriver;
import org.openqa.selenium.WebElement;
import static org.testng.Assert.assertEquals;

import static es.tid.cosmos.tests.frontend.om.CreateJobPage.BACK_NAME;
import static es.tid.cosmos.tests.frontend.om.CreateJobPage.CANCEL_LINK_CLASS;
import static es.tid.cosmos.tests.frontend.om.CreateJobPage.NEXT_NAME;

/**
 *
 * @author ximo
 */
public class SetParametersPage {

    public static final String DATASET_INPUT_ID = "dataset_path";

    private WebDriver driver;
    private final String setParametersUrl;

    public SetParametersPage(WebDriver driver) {
        this.driver = driver;
        this.setParametersUrl = driver.getCurrentUrl();
    }

    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.setParametersUrl);
    }

    public void setParameter(String paramName, String value) {
        assertCorrectUrl();
        this.driver.findElement(By.id("id_" + paramName)).sendKeys(value);
    }

    public String getParameter(String paramName) {
        assertCorrectUrl();
        return this.driver.findElement(By.id("id_" + paramName)).getAttribute(
                "value");
    }

    public void setDatasetPath(String path) {
        this.setParameter(DATASET_INPUT_ID, path);
    }

    public ReviewSettings next() {
        assertCorrectUrl();
        WebElement nextElement = this.driver.findElement(
                By.name(NEXT_NAME));
        nextElement.click();
        return new ReviewSettings(this.driver);
    }

    public CreateJobPage back() {
        assertCorrectUrl();
        WebElement backElement = this.driver.findElement(
                By.name(BACK_NAME));
        backElement.click();
        return new CreateJobPage(this.driver);
    }

    public boolean hasFileChooser(String paramName) {
        assertCorrectUrl();
        return !this.driver.findElements(
                By.cssSelector("[data-choosefor=]" + paramName)).isEmpty();
    }

    public CreateJobPage cancel() {
        assertCorrectUrl();
        WebElement cancelLink = this.driver.findElement(
                By.className(CANCEL_LINK_CLASS));
        cancelLink.click();
        return new CreateJobPage(this.driver);
    }
}
