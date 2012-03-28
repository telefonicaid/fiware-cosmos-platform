package es.tid.bdp.kpicalculation.export.mongodb;

import static java.util.Arrays.asList;
import java.util.List;

import com.mongodb.hadoop.io.BSONWritable;
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
 * @author dmicol
 */
public class MongoDBExporterReducerTest {
    private MongoDBExporterReducer instance;
    private ReduceDriver<LongWritable, Text, LongWritable, BSONWritable> driver;

    @Before
    public void setUp() {
        this.instance = new MongoDBExporterReducer();
        this.driver = new ReduceDriver<LongWritable, Text, LongWritable,
                                       BSONWritable>(this.instance);
    }

    @Test
    public void testReduce() throws Exception {
        List<Pair<LongWritable, BSONWritable>> results =
                this.driver.withInput(new LongWritable(4L),
                                      asList(new Text("abc\thttp\t9")))
                .run();
        assertEquals(1, results.size());
        Pair<LongWritable, BSONWritable> result = results.get(0);
        assertEquals("[abc, http]", 
                     result.getSecond().get("attributes").toString());
        assertEquals(9L, result.getSecond().get("count"));
    }
}
