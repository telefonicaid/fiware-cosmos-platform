package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.Set;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.util.CellCatalogue;

/**
 *
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<NodeBtsDay>, LongWritable, IntWritable> {
    private static final int MIN_TOTAL_CALLS = 200;
    private static final int MAX_TOTAL_CALLS = 5000;
    
    private Set<Long> cellCatalogue = null;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.cellCatalogue = CellCatalogue.get(context.getConfiguration());
    }
    
    @Override
    public void reduce(LongWritable key,
                       Iterable<ProtobufWritable<NodeBtsDay>> values,
                       Context context)
            throws IOException, InterruptedException {
        int numCommsInfo = 0;
        int numCommsNoInfo = 0;
        int numCommsNoBts = 0;
        for (ProtobufWritable<NodeBtsDay> value : values) {
            final NodeBtsDay nodeBtsDay = value.get();
            if (!nodeBtsDay.hasPlaceId() || nodeBtsDay.getPlaceId() == 0) {
                numCommsNoInfo++;
            } else if (!this.cellCatalogue.contains(nodeBtsDay.getPlaceId())) {
                numCommsNoBts++;
            } else {
                numCommsInfo += nodeBtsDay.getCount();
            }
        }
        int totalComms = numCommsInfo + numCommsNoInfo + numCommsNoBts;
        if (totalComms >= MIN_TOTAL_CALLS && totalComms <= MAX_TOTAL_CALLS) {
            context.write(key, new IntWritable(numCommsInfo));
        }
    }
}
