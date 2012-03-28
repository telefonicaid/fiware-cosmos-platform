package es.tid.ps.mobility.jobs.parsing;

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

import es.tid.ps.mobility.MobilityMain;
import es.tid.ps.mobility.mapreduce.ParseCellsMapper;

/**
 *
 * @author dmicol
 */
public class ParseCellsJob extends Job {
    private static final String JOB_NAME = "ParseCells";

    public ParseCellsJob(Configuration conf) throws IOException {
        super(conf, JOB_NAME);

        this.setJarByClass(MobilityMain.class);
        this.setInputFormatClass(TextInputFormat.class);
        this.setMapOutputKeyClass(IntWritable.class);
        this.setMapOutputValueClass(Text.class);
        this.setOutputKeyClass(LongWritable.class);
        this.setOutputValueClass(ProtobufWritable.class);
        this.setOutputFormatClass(SequenceFileOutputFormat.class);
        this.setMapperClass(ParseCellsMapper.class);
    }

    public void configure(Path inputCellsPath, Path parsedCellsPath)
            throws IOException {
        FileInputFormat.addInputPath(this, inputCellsPath);
        FileOutputFormat.setOutputPath(this, parsedCellsPath);
    }
}
