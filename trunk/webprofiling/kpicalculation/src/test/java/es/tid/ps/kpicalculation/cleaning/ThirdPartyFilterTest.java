/**
 * 
 */
package es.tid.ps.kpicalculation.cleaning;

import static org.junit.Assert.*;
import junit.framework.TestCase;

import org.apache.hadoop.conf.Configuration;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

/**
 * @author javierb
 * 
 */
public class ThirdPartyFilterTest extends TestCase {
    private IKpiCalculationFilter filter;
    private Configuration conf;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        conf = new Configuration();
        conf.addResource("kpi-filtering.xml");
        filter = new ThirdPartyFilter(conf);
    }

    /**
     * Test method for
     * {@link es.tid.ps.kpicalculation.cleaning.ThirdPartyFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterForbiddenDomain() {
        String input = "http://sexsearch.com/video";

        try {
            filter.filter(input);
            fail("A forbidden Third Party domain is passing the filter");
        } catch (KpiCalculationFilterException ex) {
            assertEquals("The URL provided belongs to third party domain",
                    ex.getMessage());
            assertEquals(ex.getCounter(),
                    KpiCalculationCounter.LINE_FILTERED_3RDPARTY);
        }

    }

    /**
     * Test method for
     * {@link es.tid.ps.kpicalculation.cleaning.ThirdPartyFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterAllowedDomain() {
        String input = "http://www.tid.es/music.mp3";

        try {
            filter.filter(input);
        } catch (IllegalStateException ex) {
            fail("An allowed domain has been filtered");
        }

    }

}
