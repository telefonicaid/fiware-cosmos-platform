package es.tid.cosmos.samples.wordcount;

import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Mapper for word counts
 *
 * @author logc
 */
public class WordCountMapper extends Mapper<LongWritable, Text,
                                            Text, IntWritable> {
    private static final IntWritable ONE = new IntWritable(1);
    
    private Text word;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.word = new Text();
    }
        
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        StringTokenizer tokenizer = new StringTokenizer(value.toString());
        while (tokenizer.hasMoreTokens()) {
            this.word.set(tokenizer.nextToken());
            context.write(this.word, ONE);
        }
    }
}
