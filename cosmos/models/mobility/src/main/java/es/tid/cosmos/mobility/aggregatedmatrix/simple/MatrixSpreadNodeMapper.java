package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ItinTime>
 * Output: <Long, ItinTime>
 * 
 * @author dmicol
 */
public class MatrixSpreadNodeMapper extends Mapper<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodbts = key.get();
        context.write(new LongWritable(nodbts.getNum1()), value);
    }
}
