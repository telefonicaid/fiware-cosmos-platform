package es.tid.cosmos.kpicalculation.cleaning;

import java.net.URI;
import java.net.URISyntaxException;

import org.apache.hadoop.conf.Configuration;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * extension. Any URL which does not contain a forbidden extension in its path
 * will pass the filter, and will call the next filter of the
 * "chain of responsibility" if exists.
 * 
 * @author javierb
 */
public class ExtensionFilter extends AbstractKpiCalculationFilter {
    private static final String CONFIG_PARAMETER = "kpifilters.extension";
    private static final String REGULAR_EXPRESSION = "([^\\s]+(\\.(?i){0})$)";

    public ExtensionFilter(Configuration conf) {
        this.init(conf, CONFIG_PARAMETER, REGULAR_EXPRESSION);
    }

    /**
     * (non-Javadoc)
     * 
     * @see es.tid.cosmos.kpicalculation.cleaning.IKpiCalculationFilter#filter(String)
     */
    @Override
    public void filter(String s) {
        try {
            URI uri = new URI(s);
            this.setMatcher(this.getPattern().matcher(uri.getPath()));
            if (this.getMatcher().matches()) {
                throw new KpiCalculationFilterException(
                        "The URL provided has a forbidden extension",
                        KpiCalculationCounter.LINE_FILTERED_EXTENSION);
            }
        } catch (URISyntaxException e) {
            throw new KpiCalculationFilterException("The URL is malformed", e,
                    KpiCalculationCounter.MALFORMED_URL);
        }
    }
}
