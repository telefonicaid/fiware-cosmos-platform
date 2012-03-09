package es.tid.bdp.samples.wordcount;

import static org.hamcrest.Matchers.*;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;
import org.junit.Before;
import org.junit.Test;

import org.apache.hadoop.io.*;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;

import es.tid.bdp.samples.wordcount.WordCountMapper;
import es.tid.bdp.samples.wordcount.WordCountReducer;

/**
 * Test case for WordCount sample model
 *
 * @author logc
 */
public class WordCountTest {

    private WordCountMapper mapper;
    private WordCountReducer reducer;
    private MapReduceDriver driver;

    @Before
    public void setUp() {
        this.mapper = new WordCountMapper();
        this.reducer = new WordCountReducer();
        this.driver = new MapReduceDriver<LongWritable, Text, Text,
            IntWritable, Text, IntWritable>();
    }

    @Test
    public void shouldCountWords() {
        Text a_short_text = new Text("una cadena de texto donde se repite una palabra");

        this.driver
            .withInput(new LongWritable(1), a_short_text)
            .withMapper(this.mapper)
            .withReducer(this.reducer)
            .withOutput(new Text("cadena"), new IntWritable(1))
            .withOutput(new Text("de"), new IntWritable(1))
            .withOutput(new Text("donde"), new IntWritable(1))
            .withOutput(new Text("palabra"), new IntWritable(1))
            .withOutput(new Text("repite"), new IntWritable(1))
            .withOutput(new Text("se"), new IntWritable(1))
            .withOutput(new Text("texto"), new IntWritable(1))
            .withOutput(new Text("una"), new IntWritable(2)) // this is the repeated word
            .runTest();
    }
}
