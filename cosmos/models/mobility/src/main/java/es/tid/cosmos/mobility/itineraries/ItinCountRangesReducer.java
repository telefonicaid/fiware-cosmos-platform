package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ItinCountRangesReducer extends Reducer<ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>, ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        double numMoves = 0.0D;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            numMoves += mobData.getDouble();
        }
        key.setConverter(ItinRange.class);
        final ItinRange moveRange = key.get();
        ProtobufWritable<MobData> distMoves = MobDataUtil.createAndWrap(
                ItinPercMoveUtil.create(moveRange.getGroup(),
                                        moveRange.getRange(), numMoves));
        ItinRange.Builder outMoveRange = ItinRange.newBuilder(moveRange);
        outMoveRange.setGroup(0);
        outMoveRange.setRange(0);
	context.write(ItinRangeUtil.wrap(outMoveRange.build()), distMoves);
    }
}
