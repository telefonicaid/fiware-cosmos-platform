package es.tid.bdp.kpicalculation.config;

import static org.hamcrest.Matchers.hasItems;
import static org.junit.Assert.assertThat;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author sortega
 */
public class KpiConfigTest {
    private KpiConfig instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new KpiConfig();
    }

    @Test
    public void shouldReadConfigFromURL() throws Exception {
        this.instance.read(getClass().getResource("kpi.properties"));
        assertThat(this.instance.getKpis(), hasItems(
                new JobDetails("NAME1", new String[] { "field1" }),
                new JobDetails("NAME2", new String[] {"field1", "field2"}),
                new JobDetails("NAME3", new String[] {"field1", "field2"},
                               "group")));
    }
}
