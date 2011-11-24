package es.tid.ps.kpicalculation.cleaning;

import org.apache.hadoop.conf.Configuration;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * domain. Any URL which is not a personal info domain ( which are not useful
 * for web profiling purposes) will pass the filter, and will call the next
 * filter of the "chain of responsibility" if exists.
 * 
 * @author javierb
 */
public class PersonalInfoFilter extends AbstractKpiCalculationFilter {
    private static final String CONFIG_PARAMETER = "kpifilters.personalinfo";
    private static final String REGULAR_EXPRESSION = "([^\\s]+{0}[^\\s]*)";

    public PersonalInfoFilter(Configuration conf) {
        super(conf, CONFIG_PARAMETER, REGULAR_EXPRESSION);
    }

    /**
     * (non-Javadoc)
     * 
     * @see es.tid.ps.kpicalculation.cleaning.IKpiCalculationFilter#filter(String)
     */
    @Override
    public void filter(String s) throws KpiCalculationFilterException {
        this.matcher = this.pattern.matcher(s);
        if (this.matcher.matches()) {
            throw new KpiCalculationFilterException(
                    "The URL provided belongs to a personal info domain",
                    KpiCalculationCounter.LINE_FILTERED_PERSONAL_INFO);
        }
    }
}
