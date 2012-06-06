package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinPercMove;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <ItinRange, ItinPercMove>
 * Output: <ItinRange, ClusterVector>
 * 
 * @author dmicol
 */
public class ItinGetVectorReducer extends Reducer<ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>, ProtobufWritable<ItinRange>,
        ProtobufWritable<MobData>> {
    private double minItinMoves;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.minItinMoves = conf.getFloat(Config.MIN_ITIN_MOVES,
                                          Float.MIN_VALUE);
    }
    
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
            j = (j >= 0) ? j : 6;  // Sunday at the end
            j *= 24;
            j += percMoves.getRange();
            distMoves.setComs(j, percMoves.getPercMoves());
            numMoves += percMoves.getPercMoves();
        }
        if (numMoves >= this.minItinMoves) {
            context.write(key, MobDataUtil.createAndWrap(distMoves.build()));
        }
    }
}
