package es.tid.ps.dynamicprofile.categoryextraction;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.kpicalculation.data.KpiCalculationCounter;
import es.tid.ps.kpicalculation.data.WebLog;

public class CategoryExtractionMapper extends
        Mapper<LongWritable, Text, CompositeKey, WebLog> {
    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            WebLog webLog = new WebLog();
            webLog.set(value.toString());
            CompositeKey cKey = new CompositeKey(webLog.visitorId,
                    webLog.fullUrl);
            context.write(cKey, webLog);
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
