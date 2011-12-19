package es.tid.ps.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;
import es.tid.ps.kpicalculation.data.WebLog;
import es.tid.ps.kpicalculation.data.WebLogFactory;
import es.tid.ps.kpicalculation.data.WebLogType;

/**
 * This class receives lines of a information of CDR´s files that have passed
 * the filters of web profiling module. The information is already formatted and
 * normalized, and is taken to calculate aggregate values depending on a
 * specific set of parameters Examples:
 * <ol>
 * <li>Config Parameters:
 * kpi.aggregation.fields="protocol,dateView",kpi.aggregation.group=null;</li>
 * <li>Input : {key: 1, values: 16737b1873ef03ad http http://tid.es/ tid.es /
 * null 01 12 2010 00:00:001 -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1
 * -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1 GET 304"}</li>
 * <li>Output: {key: http 01-12-2010, values: 1}</li>
 * </ol>
 * 
 * <ol>
 * <li>Config Parameters:
 * kpi.aggregation.fields="protocol,dateView",kpi.aggregation.group="visitorId";
 * </li>
 * <li>Input : {key: 1, values: 16737b1873ef03ad http http://tid.es/ tid.es /
 * null 01 12 2010 00:00:001 -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1
 * -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1 GET 304"}</li>
 * <li>Output: {key: http 01-12-2010 16737b1873ef03ad, values: 1}</li>
 * </ol>
 * 
 * @author javierb@tid.es
 */
public class KpiGenericMapper extends
        Mapper<LongWritable, Text, WebLog, IntWritable> {

    private static final IntWritable ONE = new IntWritable(1);
    private static final String MAIN_FIELDS_PARAMETER = "kpi.aggregation.fields";
    private static final String GROUP_FIELD_PARAMETER = "kpi.aggregation.group";
    private static final String TYPE_PARAMETER = "kpi.aggregation.type";

    private WebLog view;

    /**
     * Method that sets the configuration parameters to be used in order to emit
     * a specific set of fields of the input data during the map phase.
     * 
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        WebLogFactory.setKeys(
                context.getConfiguration().getStringCollection(
                        MAIN_FIELDS_PARAMETER),
                context.getConfiguration().get(GROUP_FIELD_PARAMETER), 
                WebLogType.valueOf(context.getConfiguration().get(TYPE_PARAMETER)));
        this.view = WebLogFactory.getPageView();
    }

    /**
     * @param key
     *            is the byte offset of the current line in the file;
     * @param value
     *            is the line from the file
     * @param context
     *            has the method "write()" to output the key,value pair
     */
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            this.view.set(value);
            context.getCounter(KpiCalculationCounter.LINE_STORED).increment(1L);
            context.write(this.view, ONE);
        } catch (Exception e) {
            context.getCounter(KpiCalculationCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
