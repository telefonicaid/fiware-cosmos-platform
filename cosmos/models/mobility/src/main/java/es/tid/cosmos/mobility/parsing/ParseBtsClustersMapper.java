package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;

/**
 *
 * @author dmicol
 */
public class ParseBtsClustersMapper extends Mapper<LongWritable, Text,
        IntWritable, ProtobufWritable<Cluster>> {
    private IntWritable clusterId;
    
    @Override
    public void setup(Context context) {
        this.clusterId = new IntWritable();
    }
    
    @Override
    public void map(LongWritable key, Text line, Context context)
            throws IOException, InterruptedException {
        BtsClustersParser parser = new BtsClustersParser(line.toString());
        ProtobufWritable<Cluster> cluster = ClusterUtil.wrap(parser.parse());
        this.clusterId.set(cluster.get().getLabel());
        context.write(this.clusterId, cluster);
    }
}