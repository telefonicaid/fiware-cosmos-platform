package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.MatrixRangeUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Float64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;

/**
 * Input: <MatrixRange, Double>
 * Output: <MatrixRange, ItinPercMove>
 *
 * @author dmicol
 */
public class MatrixCountRangesReducer extends Reducer<
        ProtobufWritable<MatrixRange>, MobilityWritable<Float64>,
        ProtobufWritable<MatrixRange>, MobilityWritable<ItinPercMove>> {
    @Override
    protected void reduce(ProtobufWritable<MatrixRange> key,
            Iterable<MobilityWritable<Float64>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(MatrixRange.class);
        final MatrixRange moveRange = key.get();
        double numMoves = 0.0D;
        for (MobilityWritable<Float64> value : values) {
            numMoves += value.get().getNum();
        }
        ProtobufWritable<MatrixRange> range = MatrixRangeUtil.createAndWrap(
                moveRange.getNode(), moveRange.getPoiSrc(),
                moveRange.getPoiTgt(), 0, 0);
        MobilityWritable<ItinPercMove> distMoves = new MobilityWritable<ItinPercMove>(
                ItinPercMoveUtil.create(moveRange.getGroup(),
                                        moveRange.getRange(), numMoves));
        context.write(range, distMoves);
    }
}
