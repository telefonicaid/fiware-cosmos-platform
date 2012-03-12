package es.tid.bdp.samples.wordcount;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;

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
        Text aShortText = new Text("a string of text where a word is repeated");

        this.driver
            .withInput(new LongWritable(1), aShortText)
            .withMapper(this.mapper)
            .withReducer(this.reducer)
            .withOutput(new Text("a"), new IntWritable(2)) // this is the repeated word
            .withOutput(new Text("is"), new IntWritable(1))
            .withOutput(new Text("of"), new IntWritable(1))
            .withOutput(new Text("repeated"), new IntWritable(1))
            .withOutput(new Text("string"), new IntWritable(1))
            .withOutput(new Text("text"), new IntWritable(1))
            .withOutput(new Text("where"), new IntWritable(1))
            .withOutput(new Text("word"), new IntWritable(1))
            .runTest();
    }
}
