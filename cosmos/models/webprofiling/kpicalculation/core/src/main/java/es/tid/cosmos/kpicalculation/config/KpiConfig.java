package es.tid.cosmos.kpicalculation.config;

import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;
import java.util.Properties;

/**
 *
 * @author sortega
 */
public class KpiConfig {
    private static final String GROUP_SEPARATOR = ";";
    private static final String FIELD_SEPARATOR = ",";
    private static final int FIELDS_PART = 0;
    private static final int GROUP_PART = 1;
    private final List<KpiFeature> kpis;

    public KpiConfig() {
        this.kpis = new ArrayList<KpiFeature>();
    }

    public void read(URL resource) throws IOException {
        Properties props = new Properties();
        props.load(resource.openStream());

        for (Entry entry : props.entrySet()) {
            String name = entry.getKey().toString();
            String[] parts = entry.getValue().toString().split(GROUP_SEPARATOR);
            String[] fields = parts[FIELDS_PART].split(FIELD_SEPARATOR);
            String group = (parts.length > GROUP_PART) ? parts[GROUP_PART]
                                                       : null;
            this.kpis.add(new KpiFeature(name, fields, group));
        }
    }

    public List<KpiFeature> getKpiFeatures() {
        return this.kpis;
    }
}
