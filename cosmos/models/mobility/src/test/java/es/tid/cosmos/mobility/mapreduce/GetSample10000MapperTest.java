package es.tid.cosmos.mobility.mapreduce;

import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.types.Pair;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.assertEquals;

import es.tid.cosmos.mobility.data.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cdr;
import es.tid.cosmos.mobility.mapreduce.GetSample10000Mapper;

/**
 *
 * @author dmicol
 */
public class GetSample10000MapperTest {
    private MapDriver<IntWritable, ProtobufWritable<Cdr>,
            LongWritable, ProtobufWritable<Cdr>> driver;
    
    @Before
    public void setUp() {
        this.driver = new MapDriver<IntWritable, ProtobufWritable<Cdr>,
                LongWritable, ProtobufWritable<Cdr>>(new GetSample10000Mapper());
    }

    @Test
    public void shouldSelectSample() throws Exception {
        ProtobufWritable<Cdr> cdr = CdrUtil.createAndWrap(
                2L, 5L, Date.getDefaultInstance(), Time.getDefaultInstance());
        this.driver
                .withInput(new IntWritable(92000), cdr)
                .withOutput(new LongWritable(2L), cdr)
                .runTest();
    }

    @Test
    public void shouldNotSelectSample() throws Exception {
        ProtobufWritable<Cdr> cdr = CdrUtil.createAndWrap(
                2L, 5L, Date.getDefaultInstance(), Time.getDefaultInstance());
        this.driver
                .withInput(new IntWritable(43242), cdr)
                .runTest();
    }
}
