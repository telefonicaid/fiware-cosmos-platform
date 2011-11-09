package es.tid.ps.kpicalculation.cleaning;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.conf.Configuration;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * domain. Any URL which is not a third party domain ( which are not useful for
 * web profiling purposes) will pass the filter, and will call the next filter
 * of the "chain of responsibility" if exists.
 * 
 * @author javierb
 * 
 */
public class ThirdPartyFilter extends AbstractKpiCalculationFilter {
    private static final String CONFIG_PARAMETER = "kpifilters.3rdparty";
    private static final String REGULAR_EXPRESSION = "([^\\s]+{0}[^\\s]*)";
    private static String forbiddenPattern = "";

    private Pattern pattern;
    private Matcher matcher;

    public ThirdPartyFilter(Configuration conf) {
        if (forbiddenPattern == "")
            forbiddenPattern = setPattern(REGULAR_EXPRESSION,
                    conf.get(CONFIG_PARAMETER));
        pattern = Pattern.compile(forbiddenPattern, Pattern.CASE_INSENSITIVE);
    }

    /**
     * (non-Javadoc)
     * 
     * @see es.tid.ps.kpicalculation.cleaning.IKpiCalculationFilter#filter(String)
     */
    @Override
    public void filter(String s) throws KpiCalculationFilterException {
        matcher = pattern.matcher(s);

        if (matcher.matches())
            throw new KpiCalculationFilterException(
                    "The URL provided belongs to third party domain",
                    KpiCalculationCounter.LINE_FILTERED_3RDPARTY);
    }
}