package es.tid.cosmos.samples.wordcount;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for WordCount sample model
 *
 * @author logc
 */
public class WordCountTest {
    private MapReduceDriver<LongWritable, Text, Text, IntWritable, Text,
            LongWritable> driver;

    @Before
    public void setUp() {
        this.driver = new MapReduceDriver<LongWritable, Text, Text,
                IntWritable, Text, LongWritable>(new WordCountMapper(),
                                                 new WordCountReducer());
    }

    @Test
    public void shouldCountWords() {
        this.driver
            .withConfiguration(new Configuration() {{
                set("cosmos.wordcount.delim", "\\s+");
            }})
            .withInput(new LongWritable(1L),
                       new Text("a string of text where a word is repeated"))
            .withOutput(new Text("a"), new LongWritable(2))
            .withOutput(new Text("is"), new LongWritable(1))
            .withOutput(new Text("of"), new LongWritable(1))
            .withOutput(new Text("repeated"), new LongWritable(1))
            .withOutput(new Text("string"), new LongWritable(1))
            .withOutput(new Text("text"), new LongWritable(1))
            .withOutput(new Text("where"), new LongWritable(1))
            .withOutput(new Text("word"), new LongWritable(1))
            .runTest();
    }
}
