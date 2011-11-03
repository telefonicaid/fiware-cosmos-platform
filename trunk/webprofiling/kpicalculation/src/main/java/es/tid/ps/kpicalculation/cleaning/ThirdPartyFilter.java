package es.tid.ps.kpicalculation.cleaning;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * domain. Any URL which is not a third party domain ( which are not useful for
 * web profiling purposes) will pass the filter, and will call the next filter
 * of the "chain of responsibility" if exists.
 * 
 * @author javierb
 * 
 */
public class ThirdPartyFilter extends KpiCalculationFilter {

    private Pattern pattern;
    private Matcher matcher;

    private static final String FORBIDDEN_PATTERN = "([^\\s]+(sexsearch\\.com|admeld\\.com|doubleclick\\.net|adserverplus\\.com|smartadserver\\.com)[^\\s]*)";

    public ThirdPartyFilter() {
        // TODO(javierb): This initialisation should be replaced by the one
        // using distributed cache

        super();
        pattern = Pattern.compile(FORBIDDEN_PATTERN, Pattern.CASE_INSENSITIVE);

    }

    @Override
    public boolean filter(String s) {

        matcher = pattern.matcher(s);
        if (!matcher.matches() && next != null)
            return next.filter(s);
        else
            return !matcher.matches();
    }

}