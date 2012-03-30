package es.tid.bdp.hadoopjobs.printprimes;

import java.io.IOException;

import com.mongodb.hadoop.MongoOutputFormat;
import com.mongodb.hadoop.io.BSONWritable;
import com.mongodb.hadoop.util.MongoConfigUtil;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;

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
        this.setOutputValueClass(BSONWritable.class);
        this.setMapperClass(PrimePrintMapper.class);
        this.setReducerClass(PrimePrintReducer.class);
    }
    
    public void configure(Path textPath, String outputUrl)
            throws IOException {
        this.setInputFormatClass(TextInputFormat.class);
        FileInputFormat.setInputPaths(this, textPath);
        this.setOutputFormatClass(MongoOutputFormat.class);
        MongoConfigUtil.setOutputURI(this.conf, outputUrl);
    }
}
