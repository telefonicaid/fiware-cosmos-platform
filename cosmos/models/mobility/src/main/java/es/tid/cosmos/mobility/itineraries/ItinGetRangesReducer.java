package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinRangeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ItinGetRangesReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        final long node = key.get();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final ItinMovement move = mobData.getItinMovement();
            ItinRange.Builder moveRange = ItinRange.newBuilder();
            moveRange.setNode(node);
            moveRange.setPoiSrc(move.getSource().getBts());
            moveRange.setPoiTgt(move.getTarget().getBts());
            // Calculate portion of moves by hour
            int hourSrc = move.getSource().getTime().getHour();
            int hourTgt = move.getTarget().getTime().getHour();

            if (move.getSource().getDate().getWeekday()
                    != move.getTarget().getDate().getWeekday()) {
                hourTgt += 24;
            }
            int diff = (hourTgt - hourSrc);
            if (diff == 0) {
                // Source hour and target hour are the same.
                moveRange.setRange(move.getSource().getTime().getHour());
                moveRange.setGroup(move.getSource().getDate().getWeekday());
                context.write(ItinRangeUtil.wrap(moveRange.build()),
                              MobDataUtil.createAndWrap(1.0D));
            } else {
                int minutSrc = move.getSource().getTime().getMinute();
                int minutTgt = move.getTarget().getTime().getMinute();
                int dur = (diff * 60) + (minutTgt - minutSrc);
                // Comunication in source hour
                moveRange.setRange(move.getSource().getTime().getHour());
                moveRange.setGroup(move.getSource().getDate().getWeekday());
                double percMoves = (60 - minutSrc) / dur;
                context.write(ItinRangeUtil.wrap(moveRange.build()),
                              MobDataUtil.createAndWrap(percMoves));
                
                // Comunication in target hour
                moveRange.setRange(move.getTarget().getTime().getHour());
                moveRange.setGroup(move.getTarget().getDate().getWeekday());
                percMoves = minutTgt / dur;
                context.write(ItinRangeUtil.wrap(moveRange.build()),
                              MobDataUtil.createAndWrap(percMoves));
                
                // Fill the intermediate hours
                for (int i = 1; i < diff; i++) {
                    int range = move.getSource().getTime().getHour() + i;
                    int group = move.getSource().getDate().getWeekday();
                    if (range > 23) {
                        range -= 24;
                        group++;
                    }
                    moveRange.setRange(range);
                    moveRange.setGroup(group);
                    percMoves = 60 / dur;
                    context.write(ItinRangeUtil.wrap(moveRange.build()),
                                  MobDataUtil.createAndWrap(percMoves));
                }
            }
        }
    }
}
