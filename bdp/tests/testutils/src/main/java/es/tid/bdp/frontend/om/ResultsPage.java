package es.tid.bdp.frontend.om;

import org.openqa.selenium.WebDriver;

/**
 *
 * @author ximo
 */
public class ResultsPage {
    public static final String RESULT_TABLE_ID = "job-results-table";
    
    private WebDriver driver;
    
    public ResultsPage(WebDriver driver) {
        this.driver = driver;
    }
}
