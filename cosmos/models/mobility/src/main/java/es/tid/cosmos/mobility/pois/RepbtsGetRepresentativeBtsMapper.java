package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, BtsCounter>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    private int minPercRepBts;
    private int minNumberCallsBts;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.minPercRepBts = conf.getInt(Config.MIN_PERC_REP_BTS,
                                         Integer.MIN_VALUE);
        this.minNumberCallsBts = conf.getInt(Config.MIN_NUMBER_CALLS_BTS,
                                             Integer.MIN_VALUE);
    }
    
    @Override
    public void map(LongWritable key, ProtobufWritable<MobData> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final BtsCounter counter = value.get().getBtsCounter();
        if (counter.getCount() >= this.minPercRepBts
                && counter.getRange() >= this.minNumberCallsBts) {
            ProtobufWritable<TwoInt> nodeBts = TwoIntUtil.createAndWrap(
                    key.get(), counter.getBts());
            BtsCounter btsCounter = BtsCounterUtil.create(counter.getBts(),
                    0, 0, counter.getCount());
            context.write(nodeBts, MobDataUtil.createAndWrap(btsCounter));
        }
    }
}
