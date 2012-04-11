package es.tid.cosmos.mobility.mapreduce;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashSet;
import java.util.Set;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;

/**
 *
 * @author dmicol
 */
public class RepbtsFilterNumCommsReducer extends Reducer<LongWritable,
        ProtobufWritable<NodeBtsDay>, LongWritable, IntWritable> {
    private static final int MIN_TOTAL_CALLS = 200;
    private static final int MAX_TOTAL_CALLS = 5000;
    
    private static final String CELL_CATALOGUE_FILE_NAME =
            "/user/hdfs/cells_cat.dat";
    
    private static Set<Long> cellCatalogue = null;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.loadCellsCataloge(context.getConfiguration(),
                               new Path(CELL_CATALOGUE_FILE_NAME));
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
            } else if (!cellCatalogue.contains(nodeBtsDay.getPlaceId())) {
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

    private void loadCellsCataloge(Configuration conf, Path input) {
        if (cellCatalogue != null) {
            return;
        }

        try {
            FileSystem fs = FileSystem.get(conf);
            FSDataInputStream in = fs.open(input);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            cellCatalogue = new HashSet<Long>();
            String line;
            while ((line = br.readLine()) != null) {
                String[] columns = line.split("|");
                cellCatalogue.add(Long.decode(columns[0]));
            }
            in.close();
        } catch (IOException ex) {
            ex.printStackTrace(System.err);
        } catch (NullPointerException ex) {
            ex.printStackTrace(System.err);
        }
    }
}
