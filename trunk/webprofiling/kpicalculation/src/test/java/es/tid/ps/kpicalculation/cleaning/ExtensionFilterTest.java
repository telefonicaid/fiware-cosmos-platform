package es.tid.ps.kpicalculation.cleaning;

import static org.junit.Assert.*;

import java.util.regex.Pattern;

import junit.framework.TestCase;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.ps.kpicalculation.KpiCleanerMapper;
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
    public void testFilterAllowedExtension() {
        String input = "http://www.tid.es/music.mp3";

        try {
            filter.filter(input);
        } catch (IllegalStateException ex) {
            fail("An allowed extension has been filtered");
        }

    }

}
