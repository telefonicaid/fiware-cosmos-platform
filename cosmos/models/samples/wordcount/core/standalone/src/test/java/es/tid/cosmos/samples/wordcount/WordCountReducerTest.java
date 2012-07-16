package es.tid.cosmos.samples.wordcount;

import static java.util.Arrays.asList;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 * Test case for WordCount sample model reducer
 *
 * @author logc
 */
public class WordCountReducerTest {
    private ReduceDriver<Text, IntWritable, Text, LongWritable> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<Text, IntWritable, Text, LongWritable>(
                new WordCountReducer());
    }

    @Test
    public void shouldCountUniqueWord() {
        this.driver
            .withInput(new Text("this"), asList(new IntWritable(1)))
            .withOutput(new Text("this"), new LongWritable(1L))
            .runTest();
    }

    @Test
    public void shouldCountRepeatedWords() {
        this.driver
            .withInput(new Text("a"), asList(new IntWritable(1),
                                             new IntWritable(1)))
            .withOutput(new Text("a"), new LongWritable(2L))
            .runTest();
    }
}
