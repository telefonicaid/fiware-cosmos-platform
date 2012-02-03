package es.tid.ps.profile.categoryextraction;

import java.io.IOException;
import java.util.Calendar;
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
        Mapper<LongWritable, Text, BinaryKey, UserNavigation> {
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
