package es.tid.cosmos.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

/**
 * Mapper for word counts
 *
 * @author logc, sortega
 */
public class WordCountMapper extends Mapper<LongWritable, Text,
                                            Text, IntWritable> {
    private static final String DELIMITER_KEY = "cosmos.wordcount.delim";
    private static final IntWritable ONE = new IntWritable(1);
    
    private Text word;
    private String delimRegex;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.word = new Text();
        this.delimRegex = context.getConfiguration().get(DELIMITER_KEY);
    }
        
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        for (String token: value.toString().split(this.delimRegex)) {
            this.word.set(token);
            context.write(this.word, ONE);
        }
    }
}
