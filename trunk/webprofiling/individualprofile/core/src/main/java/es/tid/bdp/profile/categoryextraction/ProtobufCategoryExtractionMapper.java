package es.tid.bdp.profile.categoryextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.log4j.Logger;

import es.tid.bdp.base.mapreduce.TernaryKey;
import es.tid.bdp.profile.generated.data.ProfileProtocol.WebProfilingLog;

/**
 * Maps from <line_no, log_line> to <[visitorId, date, url], count>
 *
 * @author dmicol, sortega
 **/
public class ProtobufCategoryExtractionMapper extends Mapper<LongWritable,
        ProtobufWritable<WebProfilingLog>, TernaryKey, LongWritable> {
    private static final Logger LOG = Logger.getLogger(
            ProtobufCategoryExtractionMapper.class);
    private static final LongWritable ONE = new LongWritable(1L);

    private TernaryKey outKey;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.outKey = new TernaryKey();
    }

    @Override
    public void map(LongWritable key, ProtobufWritable<WebProfilingLog> value,
                    Context context) {
        try {
            WebProfilingLog webLog = value.get();
            this.outKey.setPrimaryKey(webLog.getVisitorId());
            this.outKey.setSecondaryKey(webLog.getDate());
            this.outKey.setTertiaryKey(webLog.getFullUrl());
            context.write(this.outKey, this.ONE);
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
            LOG.warn("Exception mapping weblogs", ex);
        }
    }
}
