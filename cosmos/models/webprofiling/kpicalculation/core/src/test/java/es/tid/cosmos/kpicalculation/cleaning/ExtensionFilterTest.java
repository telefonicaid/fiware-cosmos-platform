package es.tid.cosmos.kpicalculation.cleaning;

import junit.framework.TestCase;

import org.apache.hadoop.conf.Configuration;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

public class ExtensionFilterTest extends TestCase {
    private IKpiCalculationFilter filter;
    private Configuration conf;

    @Before
    protected void setUp() {
        this.conf = new Configuration();
        this.conf.addResource("kpi-filtering.xml");
        this.filter = new ExtensionFilter(this.conf);
    }

    /**
     * Test method for
     * {@link es.tid.cosmos.kpicalculation.cleaning.ExtensionFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterForbiddenExtension() {
        try {
            this.filter.filter("http://www.tid.es/foto.jpg");
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
     * {@link es.tid.cosmos.kpicalculation.cleaning.ExtensionFilter#filter(java.lang.String)}
     * .
     */
    @Test
    public void testFilterAllowedExtension() throws Exception {
        this.filter.filter("http://www.tid.es/music.mp3");
    }
}
