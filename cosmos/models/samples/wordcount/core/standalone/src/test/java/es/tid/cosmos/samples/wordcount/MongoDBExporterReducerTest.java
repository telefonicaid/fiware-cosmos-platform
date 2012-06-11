package es.tid.cosmos.samples.wordcount;

import static java.util.Arrays.asList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.apache.hadoop.mrunit.types.Pair;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 * Use case for ExporterReducer
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterReducerTest {
    private ReduceDriver<LongWritable, Text, Text, LongWritable> driver;

    @Before
    public void setUp() {
        this.driver = new ReduceDriver<LongWritable, Text, Text, LongWritable>(
                new MongoDBExporterReducer());
    }

    @Test
    public void testReduce() throws Exception {
        List<Pair<Text, LongWritable>> results = this.driver
                .withInput(new LongWritable(2L),
                           asList(new Text("this\t192")))
                .run();
        assertEquals(1, results.size());
        Pair<Text, LongWritable> result = results.get(0);
        assertEquals("this", result.getFirst().toString());
        assertEquals(192L, result.getSecond().get());
    }
}
