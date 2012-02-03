package es.tid.ps.kpicalculation.utils;

import java.net.MalformedURLException;

import org.apache.hadoop.conf.Configuration;
import org.apache.nutch.net.URLNormalizers;
import org.apache.nutch.net.urlnormalizer.basic.BasicURLNormalizer;
import org.apache.nutch.net.urlnormalizer.regex.RegexURLNormalizer;
import org.apache.nutch.util.NutchConfiguration;

/**
 * Class used for normalization of url received in webprofiling module. It
 * applies two Nutch normalizers to perform the process. First is applied the
 * BasicURLNormalizer which is in charge of standard normalization processes,
 * and after it the RegexURLNormalizer which is an advanced normalizer whose
 * functionalities are defined in a configuration file.
 * 
 * @author javierb
 */
public abstract class KpiCalculationNormalizer {
    private static BasicURLNormalizer basicNorm;
    private static RegexURLNormalizer regexNorm;

    /**
     * Method that initializes the normalizers
     */
    public static void init() {
        if (basicNorm != null && regexNorm != null) {
            // Avoid unnecessary re-initializations.
            return;
        }
        basicNorm = new BasicURLNormalizer();
        Configuration conf = NutchConfiguration.create();
        basicNorm.setConf(conf);
        regexNorm = new RegexURLNormalizer(conf);
       
    }

    /**
     * Method that applies the normalizers to a URL
     * 
     * @param inputUrl
     *            url to normalize
     * @return the normalized url
     */
    public static String normalize(String inputUrl)
            throws MalformedURLException {
        String normalizedUrl = basicNorm.normalize(inputUrl,
                URLNormalizers.SCOPE_DEFAULT);
       
        // Advanced Nutch URL normalization based on regular expressions
        return regexNorm.normalize(normalizedUrl, URLNormalizers.SCOPE_DEFAULT);
    }
}
