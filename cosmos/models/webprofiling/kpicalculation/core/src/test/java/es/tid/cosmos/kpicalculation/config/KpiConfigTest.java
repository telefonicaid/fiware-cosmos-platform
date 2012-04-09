package es.tid.cosmos.kpicalculation.config;

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
        assertThat(this.instance.getKpiFeatures(), hasItems(
                new KpiFeature("NAME1", new String[] { "field1" }),
                new KpiFeature("NAME2", new String[] {"field1", "field2"}),
                new KpiFeature("NAME3", new String[] {"field1", "field2"},
                               "group")));
    }
}
