package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 * Input: <ItinRange, Double>
 * Output: <ItinRange, ItinPercMove>
 *
 * @author dmicol
 */
public class ItinCountRangesReducer extends Reducer<ProtobufWritable<ItinRange>,
        MobilityWritable<Float64>, ProtobufWritable<ItinRange>,
        MobilityWritable<ItinPercMove>> {
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<MobilityWritable<Float64>> values, Context context)
            throws IOException, InterruptedException {
        double numMoves = 0.0D;
        for (MobilityWritable<Float64> value : values) {
            numMoves += value.get().getNum();
        }
        key.setConverter(ItinRange.class);
        final ItinRange moveRange = key.get();
        MobilityWritable<ItinPercMove> distMoves = new MobilityWritable<ItinPercMove>(
                ItinPercMoveUtil.create(moveRange.getGroup(),
                                        moveRange.getRange(), numMoves));
        ItinRange.Builder outMoveRange = ItinRange.newBuilder(moveRange);
        outMoveRange.setGroup(0);
        outMoveRange.setRange(0);
        context.write(ItinRangeUtil.wrap(outMoveRange.build()), distMoves);
    }
}
