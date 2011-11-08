package es.tid.ps.kpicalculation.cleaning;

import java.net.URI;
import java.net.URISyntaxException;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.conf.Configuration;

/**
 * Class to implement the filtering of urls of CDR's inputs depending on its
 * extension. Any URL which does not contain a forbidden extension in its path
 * will pass the filter, and will call the next filter of the
 * "chain of responsibility" if exists.
 * 
 * @author javierb
 * 
 */
public class ExtensionFilter extends AbstractKpiCalculationFilter{
    private Pattern pattern;
    private Matcher matcher;

    private static final String CONFIG_PARAMETER = "kpifilters.extension";
    private static final String REGULAR_EXPRESSION = "([^\\s]+(\\.(?i){0})$)";
    private static String forbiddenPattern = "";

    public ExtensionFilter(Configuration conf) {
        if( forbiddenPattern == "" )
            forbiddenPattern = setPattern(REGULAR_EXPRESSION, conf.get(CONFIG_PARAMETER));
        pattern = Pattern.compile(forbiddenPattern, Pattern.CASE_INSENSITIVE);
    }

    /**
     * (non-Javadoc)
     * 
     * @see es.tid.ps.kpicalculation.cleaning.IKpiCalculationFilter#filter(String)
     */
    @Override
    public void filter(String s) throws IllegalStateException {
        URI uri;

        try {
            uri = new URI(s);
            matcher = pattern.matcher(uri.getPath());
        } catch (URISyntaxException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        if (matcher.matches())
            throw new IllegalStateException(
                    "The URL provided has a forbidden extension");
    }
}
