package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ItinGetVectorReducer extends Reducer<ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>, ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>> {
    private static final double MIN_ITIN_MOVES = 6.9D;
    
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        ClusterVector.Builder distMoves = ClusterVector.newBuilder();
        for (int i = 0; i < 168; i++) {
            distMoves.addComs(0.0D);
        }
        double numMoves = 0.0D;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final ItinPercMove percMoves = mobData.getItinPercMove();
            int j = percMoves.getGroup() - 1;  // Vector starts on Monday
            j = j >= 0 ? j : 6;	 // Sunday at the end
            j *= 24;
            j += percMoves.getRange();
            distMoves.setComs(j, percMoves.getPercMoves());
            numMoves += percMoves.getPercMoves();
        }
        if (numMoves >= MIN_ITIN_MOVES) {
            context.write(key, MobDataUtil.createAndWrap(distMoves.build()));
        }
    }
}
