package es.tid.cosmos.hadoopjobs.mapperfail;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

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
    
    public void configure(Path textPath, Path outputPath)
            throws IOException {
        this.setInputFormatClass(TextInputFormat.class);
        FileInputFormat.setInputPaths(this, textPath);
        this.setOutputFormatClass(TextOutputFormat.class);
        FileOutputFormat.setOutputPath(this, outputPath);
    }
}
