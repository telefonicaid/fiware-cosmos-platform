package es.tid.ps.profile.categoryextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.base.mapreduce.BinaryKey;
import es.tid.ps.profile.data.ProfileProtocol.UserNavigation;
import es.tid.ps.profile.data.UserNavigationUtil;

/**
 * Maps from <line_no, log_line> to <[visitorId, date], user_navigation>
 *
 * @author dmicol
 **/
public class CategoryExtractionMapper extends Mapper<LongWritable, Text,
        BinaryKey, ProtobufWritable<UserNavigation>> {
    private ProtobufWritable<UserNavigation> webLogWritable;
    private BinaryKey cKey;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.webLogWritable = ProtobufWritable.newInstance(
                UserNavigation.class);
        this.cKey = new BinaryKey();
    }

    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            UserNavigation webLog = UserNavigationUtil.parse(value.toString());
            this.cKey.setPrimaryKey(webLog.getUserId());
            this.cKey.setSecondaryKey(webLog.getDate());
            this.webLogWritable.set(webLog);
            context.write(this.cKey, this.webLogWritable);
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
