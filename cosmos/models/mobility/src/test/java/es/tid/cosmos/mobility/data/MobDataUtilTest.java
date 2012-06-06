package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.*;

/**
 *
 * @author dmicol
 */
public class MobDataUtilTest {
    @Test
    public void testCreateAndWrap() {
        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    NullWritable.get());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.NULL, mobData.getType());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(57);
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.INT, mobData.getType());
            assertEquals(57, mobData.getInt());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(57L);
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.LONG, mobData.getType());
            assertEquals(57L, mobData.getLong());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(57D);
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.DOUBLE, mobData.getType());
            assertEquals(57D, mobData.getDouble(), 0.0D);
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    Cdr.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.CDR, mobData.getType());
            assertEquals(Cdr.getDefaultInstance(), mobData.getCdr());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    Cell.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.CELL, mobData.getType());
            assertEquals(Cell.getDefaultInstance(), mobData.getCell());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    Bts.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.BTS, mobData.getType());
            assertEquals(Bts.getDefaultInstance(), mobData.getBts());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    Cluster.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.CLUSTER, mobData.getType());
            assertEquals(NodeBtsDay.getDefaultInstance(),
                         mobData.getNodeBtsDay());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    NodeBtsDay.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.NODE_BTS_DAY, mobData.getType());
            assertEquals(NodeBtsDay.getDefaultInstance(),
                         mobData.getNodeBtsDay());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    BtsCounter.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.BTS_COUNTER, mobData.getType());
            assertEquals(BtsCounter.getDefaultInstance(),
                         mobData.getBtsCounter());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    Poi.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.POI, mobData.getType());
            assertEquals(Poi.getDefaultInstance(), mobData.getPoi());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    PoiPos.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.POI_POS, mobData.getType());
            assertEquals(PoiPos.getDefaultInstance(), mobData.getPoiPos());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    MobViMobVars.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.MOB_VI_MOB_VARS, mobData.getType());
            assertEquals(MobViMobVars.getDefaultInstance(),
                         mobData.getMobViMobVars());
        }
        
        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    TwoInt.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.TWO_INT, mobData.getType());
            assertEquals(TwoInt.getDefaultInstance(), mobData.getTwoInt());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    ClusterVector.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.CLUSTER_VECTOR, mobData.getType());
            assertEquals(ClusterVector.getDefaultInstance(),
                         mobData.getClusterVector());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    PoiNew.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.POI_NEW, mobData.getType());
            assertEquals(PoiNew.getDefaultInstance(), mobData.getPoiNew());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    NodeMxCounter.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.NODE_MX_COUNTER, mobData.getType());
            assertEquals(NodeMxCounter.getDefaultInstance(),
                         mobData.getNodeMxCounter());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    DailyVector.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.DAILY_VECTOR, mobData.getType());
            assertEquals(DailyVector.getDefaultInstance(),
                         mobData.getDailyVector());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    MobVars.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.MOB_VARS, mobData.getType());
            assertEquals(MobVars.getDefaultInstance(), mobData.getMobVars());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    ItinTime.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.ITIN_TIME, mobData.getType());
            assertEquals(ItinTime.getDefaultInstance(), mobData.getItinTime());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    ItinMovement.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.ITIN_MOVEMENT, mobData.getType());
            assertEquals(ItinMovement.getDefaultInstance(),
                         mobData.getItinMovement());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    ItinRange.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.ITIN_RANGE, mobData.getType());
            assertEquals(ItinRange.getDefaultInstance(),
                         mobData.getItinRange());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    ItinPercMove.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.ITIN_PERC_MOVE, mobData.getType());
            assertEquals(ItinPercMove.getDefaultInstance(),
                         mobData.getItinPercMove());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    Itinerary.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.ITINERARY, mobData.getType());
            assertEquals(Itinerary.getDefaultInstance(),
                         mobData.getItinerary());
        }

        {
            ProtobufWritable<MobData> wrapper = MobDataUtil.createAndWrap(
                    MatrixTime.getDefaultInstance());
            wrapper.setConverter(MobData.class);
            MobData mobData = wrapper.get();
            assertEquals(MobData.Type.MATRIX_TIME, mobData.getType());
            assertEquals(MatrixTime.getDefaultInstance(),
                         mobData.getMatrixTime());
        }
    }
}
