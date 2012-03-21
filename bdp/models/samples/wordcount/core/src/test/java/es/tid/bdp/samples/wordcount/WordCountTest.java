package es.tid.bdp.samples.wordcount;

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
    private WordCountMapper mapper;
    private WordCountReducer reducer;
    private MapReduceDriver driver;

    @Before
    public void setUp() {
        this.mapper = new WordCountMapper();
        this.reducer = new WordCountReducer();
        this.driver = new MapReduceDriver<LongWritable, Text, Text,
                IntWritable, Text, LongWritable>();
    }

    @Test
    public void shouldCountWords() {
        Text aShortText = new Text("a string of text where a word is repeated");

        this.driver
            .withInput(new LongWritable(1L), aShortText)
            .withMapper(this.mapper)
            .withReducer(this.reducer)
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
