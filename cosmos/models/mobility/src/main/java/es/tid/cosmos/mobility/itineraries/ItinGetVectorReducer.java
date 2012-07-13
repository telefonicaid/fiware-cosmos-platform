package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;

/**
 * Input: <ItinRange, ItinPercMove>
 * Output: <ItinRange, ClusterVector>
 * 
 * @author dmicol
 */
public class ItinGetVectorReducer extends Reducer<
        ProtobufWritable<ItinRange>, TypedProtobufWritable<ItinPercMove>,
        ProtobufWritable<ItinRange>, TypedProtobufWritable<ClusterVector>> {
    private double minItinMoves;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.minItinMoves = conf.getItinMinMoves();
    }
    
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<TypedProtobufWritable<ItinPercMove>> values,
            Context context) throws IOException, InterruptedException {
        ClusterVector.Builder distMoves = ClusterVector.newBuilder();
        for (int i = 0; i < 168; i++) {
            distMoves.addComs(0.0D);
        }
        double numMoves = 0.0D;
        for (TypedProtobufWritable<ItinPercMove> value : values) {
            final ItinPercMove percMoves = value.get();
            int j = percMoves.getGroup() - 1;  // Vector starts on Monday
            j = (j >= 0) ? j : 6;  // Sunday at the end
            j *= 24;
            j += percMoves.getRange();
            distMoves.setComs(j, percMoves.getPercMoves());
            numMoves += percMoves.getPercMoves();
        }
        if (numMoves >= this.minItinMoves) {
            context.write(key, new TypedProtobufWritable<ClusterVector>(
                    distMoves.build()));
        }
    }
}
