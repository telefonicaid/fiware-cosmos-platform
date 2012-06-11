package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <BtsCounter, Null>
 * 
 * @author dmicol
 */
public class VectorFilterBtsMapper extends Mapper<LongWritable,
        MobilityWritable<TwoInt>, ProtobufWritable<BtsCounter>,
        MobilityWritable<Null>> {
    @Override
    protected void map(LongWritable key, MobilityWritable<TwoInt> value,
        Context context) throws IOException, InterruptedException {
        final TwoInt groupHour = value.get();
        ProtobufWritable<BtsCounter> counter = BtsCounterUtil.createAndWrap(
                key.get(), (int)groupHour.getNum1(),
                (int)groupHour.getNum2(), 0);
        context.write(counter, new MobilityWritable<Null>(Null.getDefaultInstance()));
    }
}
