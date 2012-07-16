package es.tid.cosmos.kpicalculation.cleaning;

import org.apache.hadoop.conf.Configuration;

import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * domain. Any URL which is not a third party domain ( which are not useful for
 * web profiling purposes) will pass the filter, and will call the next filter
 * of the "chain of responsibility" if exists.
 *
 * @author javierb
 */
public class ThirdPartyFilter extends AbstractKpiCalculationFilter {
    private static final String CONFIG_PARAMETER = "kpifilters.3rdparty";
    private static final String REGULAR_EXPRESSION = "([^\\s]+{0}[^\\s]*)";

    public ThirdPartyFilter(Configuration conf) {
        this.init(conf, CONFIG_PARAMETER, REGULAR_EXPRESSION);
    }

    /**
     * (non-Javadoc)
     *
     * @see es.tid.cosmos.kpicalculation.cleaning.IKpiCalculationFilter#filter(String)
     */
    @Override
    public void filter(String s) {
        this.setMatcher(this.getPattern().matcher(s));
        if (this.getMatcher().matches()) {
            throw new KpiCalculationFilterException(
                    "The URL provided belongs to third party domain",
                    KpiCalculationCounter.LINE_FILTERED_3RDPARTY);
        }
    }
}