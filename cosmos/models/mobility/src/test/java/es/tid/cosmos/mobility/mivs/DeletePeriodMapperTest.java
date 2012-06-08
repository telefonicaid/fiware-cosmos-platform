package es.tid.cosmos.mobility.mivs;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.mobility.data.CellUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TelMonthUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;

/**
 *
 * @author dmicol
 */
public class DeletePeriodMapperTest {
    private MapDriver<ProtobufWritable<TelMonth>, ProtobufWritable<MobData>,
            ProtobufWritable<TelMonth>, ProtobufWritable<MobData>> driver;
    
    @Before
    public void setUp() {
        //this.driver = new MapDriver<ProtobufWritable<TelMonth>,
        //        ProtobufWritable<MobData>, ProtobufWritable<TelMonth>,
        //        ProtobufWritable<MobData>>(new DeletePeriodMapper());
    }

    @Test
    public void testMap() throws Exception {
        ProtobufWritable<TelMonth> inputKey = TelMonthUtil.createAndWrap(
                1232121L, 7, true);
        ProtobufWritable<TelMonth> outputkey = TelMonthUtil.createAndWrap(
                1232121L, 1, true);
        ProtobufWritable<MobData> value = MobDataUtil.createAndWrap(
                CellUtil.create(1L, 2L, 3, 4, 5D, 6D));
        this.driver
                .withInput(inputKey, value)
                .withOutput(outputkey, value)
                .runTest();
    }
}
