package es.tid.bdp.mobility.jobs.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;

import es.tid.bdp.mobility.MobilityMain;
import es.tid.bdp.mobility.mapreduce.ParseCdrsMapper;

/**
 *
 * @author dmicol
 */
public class ParseCdrsJob extends Job {
    private static final String JOB_NAME = "ParseCDRs";

    public ParseCdrsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(TextInputFormat.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(Text.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setMapperClass(ParseCdrsMapper.class);
    }

    public void configure(Path inputCdrsPath, Path parsedCrdsPath)
            throws IOException {
        FileInputFormat.setInputPaths(this, inputCdrsPath);
        FileOutputFormat.setOutputPath(this, parsedCrdsPath);
    }
}
