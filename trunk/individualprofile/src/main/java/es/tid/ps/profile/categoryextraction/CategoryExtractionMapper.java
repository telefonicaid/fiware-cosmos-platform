package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.util.Calendar;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Enum with the list of counters to use in the CategoryExtraction mapreduces.
 *
 * @author dmicol
 **/
public class CategoryExtractionMapper extends
        Mapper<LongWritable, Text, CompositeKey, UserNavigation> {
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
            this.cKey.setSecondaryKey(
                    this.webLog.getDate().get(Calendar.YEAR) + "-" +
                    this.webLog.getDate().get(Calendar.MONTH) + "-" +
                    this.webLog.getDate().get(Calendar.DATE));
            context.write(this.cKey, this.webLog);
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
