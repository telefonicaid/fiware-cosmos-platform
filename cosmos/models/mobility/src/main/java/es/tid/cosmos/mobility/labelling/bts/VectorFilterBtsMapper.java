package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <BtsCounter, Null>
 * 
 * @author dmicol
 */
public class VectorFilterBtsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<BtsCounter>,
        ProtobufWritable<MobData>> {
    @Override
    protected void map(LongWritable key, ProtobufWritable<MobData> value,
        Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final TwoInt groupHour = value.get().getTwoInt();
        ProtobufWritable<BtsCounter> counter = BtsCounterUtil.createAndWrap(
                key.get(), (int)groupHour.getNum1(),
                (int)groupHour.getNum2(), 0);
        context.write(counter, MobDataUtil.createAndWrap(NullWritable.get()));
    }
}
