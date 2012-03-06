/**
 * 
 */
package es.tid.bdp.kpicalculation.cleaning;

import es.tid.bdp.kpicalculation.cleaning.IKpiCalculationFilter;
import es.tid.bdp.kpicalculation.cleaning.KpiCalculationFilterException;
import es.tid.bdp.kpicalculation.cleaning.PersonalInfoFilter;
import junit.framework.TestCase;

import org.apache.hadoop.conf.Configuration;
import org.junit.Before;
import org.junit.Test;

import es.tid.bdp.kpicalculation.data.KpiCalculationCounter;

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
        conf = new Configuration();
        conf.addResource("kpi-filtering.xml");
        filter = new PersonalInfoFilter(conf);
    }

    /**
     * Test method for
     * {@link es.tid.ps.kpicalculation.cleaning.PersonalInfoFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterForbiddenDomain() {
        String input = "http://pornhub.com/t1/video";

        try {
            filter.filter(input);
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
     * {@link es.tid.ps.kpicalculation.cleaning.PersonalInfoFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterAllowedDomain() throws Exception {
        String input = "http://www.tid.es/music.mp3";
        filter.filter(input);
    }
}
