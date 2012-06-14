package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, BtsCounter>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
class RepbtsGetRepresentativeBtsMapper extends Mapper<LongWritable,
        TypedProtobufWritable<BtsCounter>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<BtsCounter>> {
    private int minPercRepBts;
    private int minNumberCallsBts;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.minPercRepBts = conf.getInt(Config.POI_MIN_PERC_REP_BTS,
                                         Integer.MIN_VALUE);
        this.minNumberCallsBts = conf.getInt(Config.POI_MIN_NUMBER_CALLS_BTS,
                                             Integer.MIN_VALUE);
    }
    
    @Override
    public void map(LongWritable key, TypedProtobufWritable<BtsCounter> value,
            Context context) throws IOException, InterruptedException {
        final BtsCounter counter = value.get();
        if (counter.getCount() >= this.minPercRepBts
                && counter.getRange() >= this.minNumberCallsBts) {
            ProtobufWritable<TwoInt> nodeBts = TwoIntUtil.createAndWrap(
                    key.get(), counter.getBts());
            BtsCounter btsCounter = BtsCounterUtil.create(counter.getBts(),
                    0, 0, counter.getCount());
            context.write(nodeBts, new TypedProtobufWritable<BtsCounter>(btsCounter));
        }
    }
}
