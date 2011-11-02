package es.tid.ps.kpicalculation.cleaning;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ExtensionFilter extends KpiCalculationFilter {

    private Pattern pattern;
    private Matcher matcher;

    private static final String FORBIDDEN_PATTERN = "([^\\s]+(\\.(?i)(js|jpg|jpeg|css|gif|png|bmp|ico|tif|tiff))$)";

    public ExtensionFilter() {

        super();
        pattern = Pattern.compile(FORBIDDEN_PATTERN, Pattern.CASE_INSENSITIVE);

    }

    @Override
    public boolean filter(String s) {

        matcher = pattern.matcher(s);
        if ( !matcher.matches() && next != null )
            return next.filter(s);
        else
            return !matcher.matches();

    }

}
