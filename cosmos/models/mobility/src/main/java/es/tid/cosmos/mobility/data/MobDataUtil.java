package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.io.NullWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.*;

/**
 *
 * @author dmicol
 */
public abstract class MobDataUtil {
    public static MobData create(NullWritable unused) {
        return MobData.newBuilder()
                .setType(MobData.Type.NULL)
                .build();
    }
    
    public static MobData create(int value) {
        return MobData.newBuilder()
                .setType(MobData.Type.INT)
                .setInt(value)
                .build();
    }
    
    public static MobData create(long value) {
        return MobData.newBuilder()
                .setType(MobData.Type.LONG)
                .setLong(value)
                .build();
    }
    
    public static MobData create(double value) {
        return MobData.newBuilder()
                .setType(MobData.Type.DOUBLE)
                .setDouble(value)
                .build();
    }
    
    public static MobData create(Cdr cdr) {
        return MobData.newBuilder()
                .setType(MobData.Type.CDR)
                .setCdr(cdr)
                .build();
    }
    
    public static MobData create(Cell cell) {
        return MobData.newBuilder()
                .setType(MobData.Type.CELL)
                .setCell(cell)
                .build();
    }

    public static MobData create(Bts bts) {
        return MobData.newBuilder()
                .setBts(bts)
                .setType(MobData.Type.BTS)
                .build();
    }

    public static MobData create(Cluster cluster) {
        return MobData.newBuilder()
                .setType(MobData.Type.CLUSTER)
                .setCluster(cluster)
                .build();
    }

    public static MobData create(NodeBtsDay nodeBtsDay) {
        return MobData.newBuilder()
                .setType(MobData.Type.NODE_BTS_DAY)
                .setNodeBtsDay(nodeBtsDay)
                .build();
    }
    
    public static MobData create(BtsCounter btsCounter) {
        return MobData.newBuilder()
                .setType(MobData.Type.BTS_COUNTER)
                .setBtsCounter(btsCounter)
                .build();
    }

    public static MobData create(Poi poi) {
        return MobData.newBuilder()
                .setType(MobData.Type.POI)
                .setPoi(poi)
                .build();
    }

    public static MobData create(PoiPos poiPos) {
        return MobData.newBuilder()
                .setType(MobData.Type.POI_POS)
                .setPoiPos(poiPos)
                .build();
    }

    public static MobData create(MobViMobVars mobViMobVars) {
        return MobData.newBuilder()
                .setType(MobData.Type.MOB_VI_MOB_VARS)
                .setMobViMobVars(mobViMobVars)
                .build();
    }
    
    public static MobData create(TwoInt twoInt) {
        return MobData.newBuilder()
                .setType(MobData.Type.TWO_INT)
                .setTwoInt(twoInt)
                .build();
    }
    
    public static MobData create(ClusterVector clusterVector) {
        return MobData.newBuilder()
                .setType(MobData.Type.CLUSTER_VECTOR)
                .setClusterVector(clusterVector)
                .build();
    }
    
    public static MobData create(PoiNew poiNew) {
        return MobData.newBuilder()
                .setType(MobData.Type.POI_NEW)
                .setPoiNew(poiNew)
                .build();
    }

    public static MobData create(NodeMxCounter nodeMxCounter) {
        return MobData.newBuilder()
                .setType(MobData.Type.NODE_MX_COUNTER)
                .setNodeMxCounter(nodeMxCounter)
                .build();
    }

    public static MobData create(DailyVector dailyVector) {
        return MobData.newBuilder()
                .setType(MobData.Type.DAILY_VECTOR)
                .setDailyVector(dailyVector)
                .build();
    }

    public static MobData create(MobVars mobVars) {
        return MobData.newBuilder()
                .setType(MobData.Type.MOB_VARS)
                .setMobVars(mobVars)
                .build();
    }
    
    public static MobData create(ItinTime itinTime) {
        return MobData.newBuilder()
                .setType(MobData.Type.ITIN_TIME)
                .setItinTime(itinTime)
                .build();
    }

    public static MobData create(ItinMovement itinMovement) {
        return MobData.newBuilder()
                .setType(MobData.Type.ITIN_MOVEMENT)
                .setItinMovement(itinMovement)
                .build();
    }

    public static MobData create(ItinRange itinRange) {
        return MobData.newBuilder()
                .setType(MobData.Type.ITIN_RANGE)
                .setItinRange(itinRange)
                .build();
    }

    public static MobData create(ItinPercMove itinPercMove) {
        return MobData.newBuilder()
                .setType(MobData.Type.ITIN_PERC_MOVE)
                .setItinPercMove(itinPercMove)
                .build();
    }

    public static MobData create(Itinerary itinerary) {
        return MobData.newBuilder()
                .setType(MobData.Type.ITINERARY)
                .setItinerary(itinerary)
                .build();
    }

    public static MobData create(MatrixTime matrixTime) {
        return MobData.newBuilder()
                .setType(MobData.Type.MATRIX_TIME)
                .setMatrixTime(matrixTime)
                .build();
    }
    
    public static MobData create(NodeBts bts) {
        return MobData.newBuilder()
                .setType(MobData.Type.NODE_BTS)
                .setNodeBts(bts)
                .build();
    }
    
    public static ProtobufWritable<MobData> wrap(MobData obj) {
        ProtobufWritable<MobData> wrapper = ProtobufWritable.newInstance(
                MobData.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MobData> createAndWrap(NullWritable unused) {
        return wrap(create(unused));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(NodeBts obj) {
        return wrap(create(obj));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(int obj) {
        return wrap(create(obj));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(long obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(double obj) {
        return wrap(create(obj));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(Cdr obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Cell obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Bts obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Cluster obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(NodeBtsDay obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(BtsCounter obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Poi obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(PoiPos obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(MobViMobVars obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(TwoInt obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(ClusterVector obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(PoiNew obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(NodeMxCounter obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(DailyVector obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(MobVars obj) {
        return wrap(create(obj));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(ItinTime obj) {
        return wrap(create(obj));
    }
    
    public static ProtobufWritable<MobData> createAndWrap(ItinMovement obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(ItinRange obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(ItinPercMove obj) {
        return wrap(create(obj));
    }

    public static ProtobufWritable<MobData> createAndWrap(Itinerary obj) {
        return wrap(create(obj));
    }


    public static ProtobufWritable<MobData> createAndWrap(MatrixTime obj) {
        return wrap(create(obj));
    }
    
    public static MobData setInputId(MobData obj, int inputId) {
        MobData.Builder mobDataBuilder = MobData.newBuilder(obj);
        mobDataBuilder.setInputId(inputId);
        return mobDataBuilder.build();
    }
}
