package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol
 **/
public class CategoryExtractionMapper extends
        Mapper<LongWritable, Text, CompositeKey, NullWritable> {
    private UserNavigation webLog;
    private CompositeKey cKey;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.webLog = new UserNavigation();
        this.cKey = new CompositeKey();
    }

    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            this.webLog.parse(value.toString());
            this.cKey.setPrimaryKey(this.webLog.getVisitorId());
            this.cKey.setSecondaryKey(this.webLog.getFullUrl());
            context.write(this.cKey, NullWritable.get());
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
