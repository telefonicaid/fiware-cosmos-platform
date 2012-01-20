package es.tid.ps.profile.categoryextraction;

import org.apache.hadoop.io.*;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol
 **/
public class CategoryExtractionMapper extends
        Mapper<LongWritable, Text, CompositeKey, NullWritable> {
    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            UserNavigation webLog = new UserNavigation();
            webLog.set(value.toString());
            CompositeKey cKey = new CompositeKey(webLog.visitorId,
                    webLog.fullUrl);
            context.write(cKey, NullWritable.get());
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
