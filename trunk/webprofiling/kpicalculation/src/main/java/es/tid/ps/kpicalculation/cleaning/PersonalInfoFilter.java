package es.tid.ps.kpicalculation.cleaning;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.conf.Configuration;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * domain. Any URL which is not a personali info domain ( which are not useful
 * for web profiling purposes) will pass the filter, and will call the next
 * filter of the "chain of responsibility" if exists.
 * 
 * @author javierb
 * 
 */
public class PersonalInfoFilter implements IKpiCalculationFilter {
    private Pattern pattern;
    private Matcher matcher;

    private static final String FORBIDDEN_PATTERN = "([^\\s]+(pornhub\\.com)[^\\s]*)";

    public PersonalInfoFilter() {
        // TODO(javierb): This initialisation should be replaced by the one
        // using distributed cache
       
        pattern = Pattern.compile(FORBIDDEN_PATTERN, Pattern.CASE_INSENSITIVE);
    }

    /**
     * (non-Javadoc)
     * 
     * @see es.tid.ps.kpicalculation.cleaning.IKpiCalculationFilter#filter(String)
     */
    @Override
    public void filter(String s) throws IllegalStateException {
        matcher = pattern.matcher(s);

        if (matcher.matches())
            throw new IllegalStateException(
                    "The URL provided belongs to a personal info domain");
    }
}
