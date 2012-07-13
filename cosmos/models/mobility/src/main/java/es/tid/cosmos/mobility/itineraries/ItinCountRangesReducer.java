package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 * Input: <ItinRange, Double>
 * Output: <ItinRange, ItinPercMove>
 *
 * @author dmicol
 */
public class ItinCountRangesReducer extends Reducer<ProtobufWritable<ItinRange>,
        TypedProtobufWritable<Float64>, ProtobufWritable<ItinRange>,
        TypedProtobufWritable<ItinPercMove>> {
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<TypedProtobufWritable<Float64>> values, Context context)
            throws IOException, InterruptedException {
        double numMoves = 0.0D;
        for (TypedProtobufWritable<Float64> value : values) {
            numMoves += value.get().getValue();
        }
        key.setConverter(ItinRange.class);
        final ItinRange moveRange = key.get();
        TypedProtobufWritable<ItinPercMove> distMoves = new TypedProtobufWritable<ItinPercMove>(
                ItinPercMoveUtil.create(moveRange.getGroup(),
                                        moveRange.getRange(), numMoves));
        ItinRange.Builder outMoveRange = ItinRange.newBuilder(moveRange);
        outMoveRange.setGroup(0);
        outMoveRange.setRange(0);
        context.write(ItinRangeUtil.wrap(outMoveRange.build()), distMoves);
    }
}
