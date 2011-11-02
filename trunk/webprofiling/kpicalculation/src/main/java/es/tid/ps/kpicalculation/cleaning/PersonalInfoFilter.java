package es.tid.ps.kpicalculation.cleaning;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class PersonalInfoFilter extends KpiCalculationFilter {

    private Pattern pattern;
    private Matcher matcher;

    private static final String FORBIDDEN_PATTERN = "([^\\s]+(pornhub\\.com)[^\\s]*)";

    public PersonalInfoFilter() {
        // TODO(javierb): This initialisation should be replaced by the one
        // using distributed cache

        super();
        pattern = Pattern.compile(FORBIDDEN_PATTERN, Pattern.CASE_INSENSITIVE);

    }

    @Override
    public boolean filter(String s) {
        // TODO(javierb): Include normalization process to the input string

        matcher = pattern.matcher(s);
        if (!matcher.matches() && next != null)
            return next.filter(s);
        else
            return !matcher.matches();

    }

}
