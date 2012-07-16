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
public class PersonalInfoFilterTest extends TestCase {
    private IKpiCalculationFilter filter;
    private Configuration conf;

    /**
     * @throws java.lang.Exception
     */
    @Before
    public void setUp() throws Exception {
        this.conf = new Configuration();
        this.conf.addResource("kpi-filtering.xml");
        this.filter = new PersonalInfoFilter(this.conf);
    }

    /**
     * Test method for
     * {@link es.tid.cosmos.kpicalculation.cleaning.PersonalInfoFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterForbiddenDomain() {
        try {
            this.filter.filter("http://pornhub.com/t1/video");
            fail("A forbidden Personal Info domain is passing the filter");
        } catch (KpiCalculationFilterException ex) {
            assertEquals("The URL provided belongs to a personal info domain",
                    ex.getMessage());
            assertEquals(ex.getCounter(),
                    KpiCalculationCounter.LINE_FILTERED_PERSONAL_INFO);
        }
    }

    /**
     * Test method for
     * {@link es.tid.cosmos.kpicalculation.cleaning.PersonalInfoFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterAllowedDomain() throws Exception {
        filter.filter("http://www.tid.es/music.mp3");
    }
}
