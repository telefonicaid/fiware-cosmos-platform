package es.tid.cosmos.kpicalculation.cleaning;

import junit.framework.TestCase;

import org.apache.hadoop.conf.Configuration;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

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
        this.conf = new Configuration();
        this.conf.addResource("kpi-filtering.xml");
        this.filter = new ThirdPartyFilter(this.conf);
    }

    /**
     * Test method for
     * {@link es.tid.cosmos.kpicalculation.cleaning.ThirdPartyFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterForbiddenDomain() {
        try {
            this.filter.filter("http://sexsearch.com/video");
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
     * {@link es.tid.cosmos.kpicalculation.cleaning.ThirdPartyFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterAllowedDomain() throws Exception {
        filter.filter("http://www.tid.es/music.mp3");
    }
}
