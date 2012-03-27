package es.tid.bdp.hadoopjobs.mapperfail;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;

/**
 *
 * @author ximo
 */
public class MapperFailJob extends Job {
    private static final String JOB_NAME = "TestJar";
    
    public MapperFailJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MapperFailTool.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(IntWritable.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(IntWritable.class);
        this.setMapperClass(MapperFailMapper.class);
    }
}
