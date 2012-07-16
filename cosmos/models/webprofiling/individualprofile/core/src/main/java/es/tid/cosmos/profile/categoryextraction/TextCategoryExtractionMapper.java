package es.tid.cosmos.profile.categoryextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.log4j.Logger;

import es.tid.cosmos.base.mapreduce.TernaryKey;
import es.tid.cosmos.profile.data.UserNavigationUtil;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.UserNavigation;

/**
 * Maps from <line_no, log_line> to <[visitorId, date, url], count>
 *
 * @author dmicol
 **/
public class TextCategoryExtractionMapper extends Mapper<LongWritable, Text,
        TernaryKey, LongWritable> {
    private static final Logger LOG = Logger.getLogger(
            TextCategoryExtractionMapper.class);
    private static final LongWritable ONE = new LongWritable(1L);

    private TernaryKey outKey;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.outKey = new TernaryKey();
    }

    @Override
    public void map(LongWritable key, Text value, Context context) {
        try {
            UserNavigation webLog = UserNavigationUtil.parse(value.toString());
            this.outKey.setPrimaryKey(webLog.getUserId());
            this.outKey.setSecondaryKey(webLog.getDate());
            this.outKey.setTertiaryKey(webLog.getUrl());
            context.write(this.outKey, ONE);
        } catch (Exception ex) {
            context.getCounter(CategoryExtractionCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
            LOG.warn("Exception mapping weblogs", ex);
        }
    }
}
