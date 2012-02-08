package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import org.apache.avro.mapred.AvroKey;
import org.apache.avro.mapred.AvroValue;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.base.mapreduce.BinaryKey;

/**
 * Maps from <line_no, log_line> to <[visitorId, date], user_navigation>
 *
 * @author dmicol
 **/
public class CategoryExtractionMapper extends
        Mapper<LongWritable, Text, AvroKey<BinaryKey>,
                AvroValue<UserNavigation>> {
    private UserNavigation webLog;
    private BinaryKey cKey;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.webLog = new UserNavigation();
        this.cKey = new BinaryKey();
    }

    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            UserNavigationFactory.set(this.webLog, value.toString());
            this.cKey.setPrimaryKey(this.webLog.getVisitorId().toString());
            this.cKey.setSecondaryKey(this.webLog.getDate().toString());
            context.write(new AvroKey(this.cKey), new AvroValue(this.webLog));
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
