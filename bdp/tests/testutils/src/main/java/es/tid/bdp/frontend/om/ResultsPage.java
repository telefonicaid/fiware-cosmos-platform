package es.tid.bdp.frontend.om;

import org.openqa.selenium.WebDriver;
import static org.testng.Assert.assertEquals;

/**
 *
 * @author ximo
 */
public class ResultsPage {
    public static final String RESULT_TABLE_ID = "job-results-table";
    
    private WebDriver driver;
    private final String resultPageUrl;
    
    private void assertCorrectUrl() {
        assertEquals(this.driver.getCurrentUrl(), this.resultPageUrl);
    }
    
    public ResultsPage(WebDriver driver) {
        this.driver = driver;
        this.resultPageUrl = driver.getCurrentUrl();
    }
    
    public String[][] getResults() {
        assertCorrectUrl();
        
        throw new UnsupportedOperationException("Not yet implemented");
    }
}
