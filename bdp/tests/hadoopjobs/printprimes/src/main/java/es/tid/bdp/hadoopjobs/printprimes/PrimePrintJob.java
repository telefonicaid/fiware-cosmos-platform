package es.tid.bdp.hadoopjobs.printprimes;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;

/**
 *
 * @author ximo
 */
public class PrimePrintJob extends Job {
    private static final String JOB_NAME = "TestJar";
    
    public PrimePrintJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(PrimePrintTool.class);
        this.setMapOutputKeyClass(Text.class);
        this.setMapOutputValueClass(IntWritable.class);
        this.setOutputKeyClass(Text.class);
        this.setOutputValueClass(LongWritable.class);
        this.setMapperClass(PrimePrintMapper.class);
        this.setReducerClass(PrimePrintReducer.class);
    }
}
