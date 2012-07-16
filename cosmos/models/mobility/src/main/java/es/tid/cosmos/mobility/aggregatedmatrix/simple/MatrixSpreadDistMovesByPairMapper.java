package es.tid.cosmos.mobility.aggregatedmatrix.simple;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 * Input: <ItinRange, Double>
 * Output: <ItinRange, Double>
 *
 * @author dmicol
 */
class MatrixSpreadDistMovesByPairMapper extends Mapper<
        ProtobufWritable<ItinRange>, TypedProtobufWritable<Float64>,
        ProtobufWritable<ItinRange>, TypedProtobufWritable<Float64>> {
    @Override
    protected void map(ProtobufWritable<ItinRange> key,
            TypedProtobufWritable<Float64> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(ItinRange.class);
        final ItinRange itrang = key.get();
        ItinRange.Builder outItrang = ItinRange.newBuilder(itrang);
        outItrang.setNode(itrang.getPoiSrc() * itrang.getPoiTgt());
        context.write(ItinRangeUtil.wrap(outItrang.build()), value);
    }
}
