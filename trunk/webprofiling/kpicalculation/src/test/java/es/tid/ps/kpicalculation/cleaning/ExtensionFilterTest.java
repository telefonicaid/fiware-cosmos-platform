package es.tid.ps.kpicalculation.cleaning;

import junit.framework.TestCase;

import org.apache.hadoop.conf.Configuration;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

public class ExtensionFilterTest extends TestCase {
    private IKpiCalculationFilter filter;
    private Configuration conf;

    @Before
    protected void setUp() {
        conf = new Configuration();
        conf.addResource("kpi-filtering.xml");
        filter = new ExtensionFilter(conf);
    }

    /**
     * Test method for
     * {@link es.tid.ps.kpicalculation.cleaning.ExtensionFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterForbiddenExtension() {
        String input = "http://www.tid.es/foto.jpg";

        try {
            filter.filter(input);
            fail("A forbidden extension is passing the filter");
        } catch (KpiCalculationFilterException ex) {
            assertEquals("The URL provided has a forbidden extension",
                    ex.getMessage());
            assertEquals(ex.getCounter(),
                    KpiCalculationCounter.LINE_FILTERED_EXTENSION);
        }
    }

    /**
     * Test method for
     * {@link es.tid.ps.kpicalculation.cleaning.ExtensionFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterAllowedExtension() throws Exception {
        String input = "http://www.tid.es/music.mp3";

        filter.filter(input);
    }
}
