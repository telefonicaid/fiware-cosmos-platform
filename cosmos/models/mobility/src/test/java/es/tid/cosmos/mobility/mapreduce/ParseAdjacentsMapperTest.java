package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.mapreduce.ParseAdjacentsMapper;

/**
 *
 * @author dmicol
 */
public class ParseAdjacentsMapperTest {
    private MapDriver<IntWritable, Text, ProtobufWritable<TwoInt>, NullWritable>
            driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<IntWritable, Text, ProtobufWritable<TwoInt>,
                NullWritable>(new ParseAdjacentsMapper());
    }

    @Test
    public void test() throws IOException {
        List<Pair<ProtobufWritable<TwoInt>, NullWritable>> results = this.driver
                .withInput(new IntWritable(1),
                           new Text("12|34"))
                .run();
        assertEquals(1, results.size());
        ProtobufWritable<TwoInt> wrapper = results.get(0).getFirst();
        wrapper.setConverter(TwoInt.class);
        assertNotNull(wrapper.get());
        TwoInt obj = wrapper.get();
        assertEquals(12L, obj.getNum1());
        assertEquals(34L, obj.getNum2());
    }
}
