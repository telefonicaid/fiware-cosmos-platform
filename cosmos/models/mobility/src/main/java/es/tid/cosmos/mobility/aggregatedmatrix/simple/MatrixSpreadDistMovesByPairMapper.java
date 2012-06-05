package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <ItinRange, Double>
 * Output: <ItinRange, Double>
 * 
 * @author dmicol
 */
public class MatrixSpreadDistMovesByPairMapper extends Mapper<
        ProtobufWritable<ItinRange>, ProtobufWritable<MobData>,
        ProtobufWritable<ItinRange>, ProtobufWritable<MobData>> {
    @Override
    protected void map(ProtobufWritable<ItinRange> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(ItinRange.class);
        final ItinRange itrang = key.get();
        ItinRange.Builder outItrang = ItinRange.newBuilder(itrang);
        outItrang.setNode(itrang.getPoiSrc() * itrang.getPoiTgt());
        context.write(ItinRangeUtil.wrap(outItrang.build()), value);
    }
}
