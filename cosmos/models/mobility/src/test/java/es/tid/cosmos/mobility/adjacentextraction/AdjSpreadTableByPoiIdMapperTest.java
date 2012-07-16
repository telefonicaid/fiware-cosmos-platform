package es.tid.cosmos.mobility.adjacentextraction;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapperTest {
    private MapDriver<LongWritable, TypedProtobufWritable<TwoInt>, LongWritable,
            TypedProtobufWritable<Int64>> driver;

    @Before
    public void setUp() {
        this.driver = new MapDriver<LongWritable, TypedProtobufWritable<TwoInt>,
                LongWritable, TypedProtobufWritable<Int64>>(
                        new AdjSpreadTableByPoiIdMapper());
    }

    @Test
    public void testMap() {
        this.driver
                .withInput(new LongWritable(57L),
                           new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(3L, 6L)))
                .withOutput(new LongWritable(3L), TypedProtobufWritable.create(6L))
                .runTest();
    }
}
