package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import org.apache.hadoop.io.LongWritable;

/**
 *
 * @author dmicol
 */
public class PoiFilterSechomeAdjacentMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, LongWritable> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final MobData mobData = value.get();
        switch (mobData.getType()) {
            case LONG:
                context.write(key, new LongWritable(mobData.getLong()));
                break;
            case NULL:
                key.setConverter(TwoInt.class);
                final String keyStr = TwoIntUtil.toString(key.get());
                context.getCounter(Counters.NULL_VALUES.name(), keyStr)
                        .increment(1L);
                break;
            default:
                throw new IllegalStateException("Unexpected MobData type: "
                        + mobData.getType().name());
        }
    }
}
