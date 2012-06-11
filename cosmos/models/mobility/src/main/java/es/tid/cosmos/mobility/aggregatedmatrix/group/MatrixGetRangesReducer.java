package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MatrixRangeUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;

/**
 * Input: <Long, ItinMovement>
 * Output: <MatrixRange, Double>
 *
 * @author dmicol
 */
public class MatrixGetRangesReducer extends Reducer<LongWritable,
        MobilityWritable<ItinMovement>, ProtobufWritable<MatrixRange>,
        MobilityWritable<Float64>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<ItinMovement>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<ItinMovement> value : values) {
            final ItinMovement move = value.get();
            final ItinTime source = move.getSource();
            final ItinTime target = move.getTarget();
            MatrixRange.Builder moveRange = MatrixRange.newBuilder();
            MatrixRange.Builder moveRangeFinal;
            double percMoves;
            
            moveRange.setPoiSrc(source.getBts());
            moveRange.setPoiTgt(target.getBts());
            moveRange.setNode(moveRange.getPoiSrc() * moveRange.getPoiTgt());
            // Calculate portion of moves by hour
            int hourSrc = source.getTime().getHour();
            int hourTgt = target.getTime().getHour();

            if (source.getDate().getWeekday() !=
                    target.getDate().getWeekday()) {
                hourTgt += 24;
            }
            int diff = hourTgt - hourSrc;
            if (diff == 0) {
                moveRange.setRange(source.getTime().getHour());
                moveRange.setGroup(source.getDate().getWeekday());
                percMoves = 1.0D;
                moveRangeFinal = moveRange.clone();
                context.write(MatrixRangeUtil.wrap(moveRangeFinal.build()),
                              MobilityWritable.create(percMoves));
            } else {
                int minutSrc = source.getTime().getMinute();
                int minutTgt = target.getTime().getMinute();
                double dur = (diff * 60) + (minutTgt - minutSrc);
                // Comunication in source hour
                moveRange.setRange(source.getTime().getHour());
                moveRange.setGroup(source.getDate().getWeekday());
                percMoves = (60 - minutSrc) / dur;
                moveRangeFinal = moveRange.clone();
                context.write(MatrixRangeUtil.wrap(moveRangeFinal.build()),
                              MobilityWritable.create(percMoves));
                // Comunication in target hour
                moveRange.setRange(target.getTime().getHour());
                moveRange.setGroup(target.getDate().getWeekday());
                percMoves = minutTgt / dur;
                moveRangeFinal = moveRange.clone();
                context.write(MatrixRangeUtil.wrap(moveRangeFinal.build()),
                              MobilityWritable.create(percMoves));
                // Fill the intermediate hours
                for (int i = 1; i < diff; i++) {
                    moveRange.setRange(source.getTime().getHour() + i);
                    moveRange.setGroup(source.getDate().getWeekday());
                    if (moveRange.getRange() > 23) {
                        moveRange.setRange(moveRange.getRange() - 24);
                        moveRange.setGroup(moveRange.getGroup() + 1);
                    }
                    percMoves = 60 / dur;
                    moveRangeFinal = moveRange.clone();
                    context.write(MatrixRangeUtil.wrap(moveRangeFinal.build()),
                                  MobilityWritable.create(percMoves));
                }
            }
        }
    }
}
