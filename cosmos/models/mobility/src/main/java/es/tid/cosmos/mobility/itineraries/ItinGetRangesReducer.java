package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Float64;
import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;

/**
 * Input: <Long, ItinMovement>
 * Output: <ItinRange, Double>
 * 
 * @author dmicol
 */
public class ItinGetRangesReducer extends Reducer<LongWritable,
        TypedProtobufWritable<ItinMovement>, ProtobufWritable<ItinRange>,
        TypedProtobufWritable<Float64>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<ItinMovement>> values, Context context)
            throws IOException, InterruptedException {
        final long node = key.get();
        for (TypedProtobufWritable<ItinMovement> value : values) {
            final ItinMovement move = value.get();
            final ItinTime source = move.getSource();
            final ItinTime target = move.getTarget();
            ItinRange.Builder moveRange = ItinRange.newBuilder();
            ItinRange.Builder moveRangeFinal;
            moveRange.setNode(node);
            moveRange.setPoiSrc(source.getBts());
            moveRange.setPoiTgt(target.getBts());
            // Calculate portion of moves by hour
            int hourSrc = source.getTime().getHour();
            int hourTgt = target.getTime().getHour();

            if (source.getDate().getWeekday()
                    != target.getDate().getWeekday()) {
                hourTgt += 24;
            }
            int diff = (hourTgt - hourSrc);
            if (diff == 0) {
                // Source hour and target hour are the same.
                moveRange.setRange(source.getTime().getHour());
                moveRange.setGroup(source.getDate().getWeekday());
                moveRangeFinal = moveRange.clone();
                context.write(ItinRangeUtil.wrap(moveRangeFinal.build()),
                              TypedProtobufWritable.create(1.0D));
            } else {
                int minutSrc = source.getTime().getMinute();
                int minutTgt = target.getTime().getMinute();
                double dur = (diff * 60) + (minutTgt - minutSrc);
                // Comunication in source hour
                moveRange.setRange(source.getTime().getHour());
                moveRange.setGroup(source.getDate().getWeekday());
                double percMoves = (60.0D - (double)minutSrc) / dur;
                moveRangeFinal = moveRange.clone();
                context.write(ItinRangeUtil.wrap(moveRangeFinal.build()),
                              TypedProtobufWritable.create(percMoves));
                
                // Comunication in target hour
                moveRange.setRange(target.getTime().getHour());
                moveRange.setGroup(target.getDate().getWeekday());
                percMoves = minutTgt / dur;
                moveRangeFinal = moveRange.clone();
                context.write(ItinRangeUtil.wrap(moveRangeFinal.build()),
                              TypedProtobufWritable.create(percMoves));
                
                // Fill the intermediate hours
                for (int i = 1; i < diff; i++) {
                    int range = source.getTime().getHour() + i;
                    int group = source.getDate().getWeekday();
                    if (range > 23) {
                        range -= 24;
                        group++;
                    }
                    moveRange.setRange(range);
                    moveRange.setGroup(group);
                    percMoves = 60.0D / dur;
                    moveRangeFinal = moveRange.clone();
                    context.write(ItinRangeUtil.wrap(moveRangeFinal.build()),
                                  TypedProtobufWritable.create(percMoves));
                }
            }
        }
    }
}
