package es.tid.cosmos.kpicalculation;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;

import com.google.protobuf.Descriptors.Descriptor;
import com.google.protobuf.Descriptors.FieldDescriptor;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.base.mapreduce.CompositeKey;
import es.tid.cosmos.base.mapreduce.SingleKey;
import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;
import es.tid.cosmos.kpicalculation.generated.data.KpiCalculationProtocol.WebProfilingLog;

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
public class KpiGenericMapper extends Mapper<NullWritable,
        ProtobufWritable<WebProfilingLog>, CompositeKey, IntWritable> {
    private static final IntWritable ONE = new IntWritable(1);
    private static final String MAIN_FIELDS_PARAMETER =
            "kpi.aggregation.fields";
    private static final String GROUP_FIELD_PARAMETER = "kpi.aggregation.group";

    private CompositeKey key;
    private List<FieldDescriptor> descriptors;
    private FieldDescriptor secondaryDescriptor;

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
        initDescriptors(context.getConfiguration());
        this.key = new CompositeKey((this.secondaryDescriptor == null) ? 1 : 2);
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
    protected void map(NullWritable key,
            ProtobufWritable<WebProfilingLog> value, Context context)
            throws IOException, InterruptedException {
        try {
            value.setConverter(WebProfilingLog.class);
            WebProfilingLog log = value.get();
            this.key.set(0, getPrimaryKeyValue(log));
            if (this.secondaryDescriptor != null) {
                this.key.set(1, getSecondaryKeyValue(log));
            }
            context.getCounter(KpiCalculationCounter.LINE_STORED).increment(1L);
            context.write(this.key, ONE);
        } catch (Exception e) {
            context.getCounter(KpiCalculationCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }

    private void initDescriptors(Configuration config) {
        this.descriptors = new ArrayList<FieldDescriptor>();
        Descriptor d = WebProfilingLog.getDescriptor();
        Collection<String> primaryKeys = config
                .getStringCollection(MAIN_FIELDS_PARAMETER);

        Iterator<String> it = primaryKeys.iterator();
        while (it.hasNext()) {
            String str = it.next();
            this.descriptors.add(d.findFieldByName(str));
        }
        this.secondaryDescriptor = d.findFieldByName(config
                .get(GROUP_FIELD_PARAMETER));
    }

    private String getPrimaryKeyValue(WebProfilingLog log) {
        String result = "";
        Iterator<FieldDescriptor> it = descriptors.iterator();
        while (it.hasNext()) {
            result += (String) log.getField(it.next()).toString();
            if (it.hasNext()) {
                result += "\t";
            }
        }
        return result;
    }

    private String getSecondaryKeyValue(WebProfilingLog log) {
        return log.getField(this.secondaryDescriptor).toString();
    }
}