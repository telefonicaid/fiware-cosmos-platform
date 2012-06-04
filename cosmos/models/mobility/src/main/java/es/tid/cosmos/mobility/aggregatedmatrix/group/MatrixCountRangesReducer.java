package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinPercMoveUtil;
import es.tid.cosmos.mobility.data.MatrixRangeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <MatrixRange, Double>
 * Output: <MatrixRange, ItinPercMove>
 *
 * @author dmicol
 */
public class MatrixCountRangesReducer extends Reducer<
        ProtobufWritable<MatrixRange>, ProtobufWritable<MobData>,
        ProtobufWritable<MatrixRange>, ProtobufWritable<MobData>> {

    @Override
    protected void reduce(ProtobufWritable<MatrixRange> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(MatrixRange.class);
        final MatrixRange moveRange = key.get();
        double numMoves = 0.0D;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            numMoves += value.get().getDouble();
        }
        ProtobufWritable<MatrixRange> range = MatrixRangeUtil.createAndWrap(
                moveRange.getNode(), moveRange.getPoiSrc(),
                moveRange.getPoiTgt(), 0, 0);
        ProtobufWritable<MobData> distMoves = MobDataUtil.createAndWrap(
                ItinPercMoveUtil.create(moveRange.getGroup(),
                moveRange.getRange(), numMoves));
        context.write(range, distMoves);
    }
}
