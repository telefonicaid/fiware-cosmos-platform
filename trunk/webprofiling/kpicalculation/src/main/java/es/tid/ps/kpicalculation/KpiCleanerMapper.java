package es.tid.ps.kpicalculation;

import java.io.IOException;
import java.net.URLDecoder;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.nutch.net.URLNormalizers;
import org.apache.nutch.net.urlnormalizer.basic.BasicURLNormalizer;
import org.apache.nutch.net.urlnormalizer.regex.RegexURLNormalizer;
import org.apache.nutch.util.NutchConfiguration;


import es.tid.ps.kpicalculation.cleaning.ThirdPartyFilter;
import es.tid.ps.kpicalculation.cleaning.ExtensionFilter;
import es.tid.ps.kpicalculation.cleaning.KpiCalculationFilter;
import es.tid.ps.kpicalculation.cleaning.PersonalInfoFilter;

/**
 * This class receives lines of information of CDR´s files that will be used in
 * the Web Profiling module. The URL field of any of those lines will be
 * replaced following the requirements specified in PS20.TECH.FUN.WPF.005 by its
 * normalized version. 
 * After the normalization of the URL, a set of filters will
 * be applied, and only in case the URL passes all the filters the map function
 * will emit the input line with the replaced field. If the URL does not pass any 
 * of the filters nothing will be emited.
 *  Examples: 
 * <ol>
 * <li>Input : {key: 1, values: http://www.example.com/%7Eusername?www=3}</li>
 * <li>Output: {key: 1, values: http://example.com}</li>
 * </ol>
 * 
 * <ol>
 * <li>Input : {key: 1, values: http://www.tid.co.uk/foto.jpg}</li>
 * <li>Output: void</li>
 * </ol>
 * 
 * 
 * @author javierb@tid.es
 */
public class KpiCleanerMapper extends Mapper<LongWritable, Text, LongWritable, Text> {

    public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {

        KpiCalculationFilter f = new ExtensionFilter()
                .setNext(new ThirdPartyFilter().setNext(new PersonalInfoFilter()));

        // Basic Nutch URL normalization
        BasicURLNormalizer normalizer = new BasicURLNormalizer();
        Configuration conf = NutchConfiguration.create();
        normalizer.setConf(conf);
        System.out.println(value);
        String res = normalizer.normalize(value.toString(), URLNormalizers.SCOPE_DEFAULT);
        System.out.println(res);

        res = URLDecoder.decode(res, "UTF-8");
        // Advanced Nutch URL normalization based on regular expressions
        RegexURLNormalizer norm = new RegexURLNormalizer(conf);

        res = norm.normalize(res, URLNormalizers.SCOPE_DEFAULT);
        System.out.println(res);

        boolean emit = f.filter(res);

        if (emit)
            context.write(key, value);

    }
}
