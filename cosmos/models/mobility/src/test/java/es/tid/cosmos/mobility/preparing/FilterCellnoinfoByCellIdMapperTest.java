package es.tid.cosmos.mobility.preparing;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.CdrUtil;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 *
 * @author dmicol
 */
public class FilterCellnoinfoByCellIdMapperTest {

    private MapDriver<LongWritable, TypedProtobufWritable<Cdr>, LongWritable,
            TypedProtobufWritable<Cdr>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<Cdr>,
                LongWritable, TypedProtobufWritable<Cdr>>(
                       new FilterCellnoinfoByCellIdMapper());
    }

    @Test
    public void testValidCellId() throws Exception {
        final TypedProtobufWritable<Cdr> value = new TypedProtobufWritable<Cdr>(
                CdrUtil.create(3L, 7L, Date.getDefaultInstance(),
                               Time.getDefaultInstance()));
        this.driver
                .withInput(new LongWritable(1L), value)
                .withOutput(new LongWritable(7L), value)
                .runTest();
    }

    @Test
    public void testInvalidCellId() throws Exception {
        final TypedProtobufWritable<Cdr> value = new TypedProtobufWritable<Cdr>(
                CdrUtil.create(3L, 0L, Date.getDefaultInstance(),
                               Time.getDefaultInstance()));
        this.driver
                .withInput(new LongWritable(1L), value)
                .runTest();
    }
}
