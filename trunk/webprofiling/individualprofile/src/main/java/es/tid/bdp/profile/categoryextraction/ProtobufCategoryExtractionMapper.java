package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.bdp.base.mapreduce.BinaryKey;
import es.tid.bdp.profile.data.ProfileProtocol.UserNavigation;
import es.tid.bdp.profile.data.ProfileProtocol.WebProfilingLog;

/**
 * Maps from <line_no, log_line> to <[visitorId, date], user_navigation>
 *
 * @author dmicol
 **/
public class ProtobufCategoryExtractionMapper extends Mapper<LongWritable,
        ProtobufWritable<WebProfilingLog>, BinaryKey,
        ProtobufWritable<UserNavigation>> {
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
    public void map(LongWritable key, ProtobufWritable<WebProfilingLog> value, Context context) {
        try {
            WebProfilingLog webLog = value.get();
            this.cKey.setPrimaryKey(webLog.getVisitorId());
            this.cKey.setSecondaryKey(webLog.getDate());
            this.webLogWritable.set(UserNavigation.newBuilder()
                    .setUserId(webLog.getVisitorId())
                    .setUrl(webLog.getFullUrl())
                    .setDate(webLog.getDate())
                    .build());
            context.write(this.cKey, this.webLogWritable);
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
