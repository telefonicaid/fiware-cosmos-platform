package es.tid.cosmos.mobility.labelling.client;

import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.DailyVector.Builder;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author ximo
 */
public class VectorCreateNodeDayhourReducerTest {
    
    private ReduceDriver<ProtobufWritable<NodeBts>, TypedProtobufWritable<TwoInt>,
            ProtobufWritable<TwoInt>, TypedProtobufWritable<DailyVector>> driver;
    
    @Before
    public void setUp() {
        this.driver = new ReduceDriver<ProtobufWritable<NodeBts>,
                TypedProtobufWritable<TwoInt>, ProtobufWritable<TwoInt>,
                TypedProtobufWritable<DailyVector>>(
                        new VectorCreateNodeDayhourReducer());
    }

    @Test
    public void testReduce() {
        final ProtobufWritable<NodeBts> key = NodeBtsUtil.createAndWrap(1L, -2L,
                                                                        3, 4);
        List<TypedProtobufWritable<TwoInt>> values =
                new LinkedList<TypedProtobufWritable<TwoInt>>();
        values.add(new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(
                    0, 1)));
        values.add(new TypedProtobufWritable<TwoInt>(TwoIntUtil.create(
                    2, 3)));
        
        final ProtobufWritable<TwoInt> outKey = TwoIntUtil.createAndWrap(
                key.get().getUserId(), key.get().getBts());
        Builder outputBuilder = DailyVector.newBuilder();
        for (int i = 0; i < 24; i++) {
            outputBuilder.addHours(TwoIntUtil.create(
                key.get().getWeekday(),
                0));
        }
        outputBuilder.setHours(0, TwoIntUtil.create(
                key.get().getWeekday(),
                1));
        outputBuilder.setHours(2, TwoIntUtil.create(
                key.get().getWeekday(),
                3));
        this.driver.withInput(key, values)
                .withOutput(outKey, new TypedProtobufWritable<DailyVector>(
                            outputBuilder.build()))
                .runTest();
    }
}
