package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.conf.MobilityConfiguration;
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
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.minPercRepBts = conf.getPoiMinPercRepBts();
        this.minNumberCallsBts = conf.getPoiMinNumberCallsBts();
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
