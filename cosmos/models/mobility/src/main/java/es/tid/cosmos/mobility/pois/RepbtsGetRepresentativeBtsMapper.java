package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <Long, BtsCounter>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class RepbtsGetRepresentativeBtsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    public void map(LongWritable key, ProtobufWritable<MobData> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final BtsCounter counter = value.get().getBtsCounter();
        if (counter.getCount() >= Config.minPercRepBts
                && counter.getRange() >= Config.minNumberCallsBts) {
            ProtobufWritable<TwoInt> nodeBts = TwoIntUtil.createAndWrap(
                    key.get(), counter.getPlaceId());
            BtsCounter btsCounter = BtsCounterUtil.create(counter.getPlaceId(),
                    0, 0, counter.getCount());
            context.write(nodeBts, MobDataUtil.createAndWrap(btsCounter));
        }
    }
}
