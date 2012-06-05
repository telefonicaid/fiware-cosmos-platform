package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, MatrixTime>
 * Output: <Long, ItinMovement>
 *
 * @author dmicol
 */
public class MatrixMoveClientReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    private static final boolean INCLUDE_INTRA_MOVES = false;
    
    private int maxMinutesInMoves;
    private int minMinutesInMoves;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.maxMinutesInMoves = conf.getInt(Config.MAX_MINUTES_IN_MOVES,
                                             Integer.MAX_VALUE);
        this.minMinutesInMoves = conf.getInt(Config.MIN_MINUTES_IN_MOVES,
                                             Integer.MIN_VALUE);
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values,
            Context context) throws IOException, InterruptedException {
        List<MatrixTime> locList = new LinkedList<MatrixTime>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            locList.add(mobData.getMatrixTime());
        }
        
        for (MatrixTime loc1 : locList) {
            int minDistance = Integer.MAX_VALUE;
            MatrixTime minDistLoc = null;
            for (MatrixTime loc2 : locList) {
                if (loc2.getGroup() == loc1.getGroup() &&
                        loc2.getBts() == loc1.getBts()) {
                    continue;
                }
                if (!INCLUDE_INTRA_MOVES &&
                        loc2.getGroup() == loc1.getGroup()) {
                    continue;
                }
                int difMonth = loc2.getDate().getMonth()
                               - loc1.getDate().getMonth();
                if (difMonth > 1) {
                    continue;
                }
                int difDay = loc2.getDate().getDay()
                             - loc1.getDate().getDay();
                int difHour = loc2.getTime().getHour()
                              - loc1.getTime().getHour();
                int difMin = loc2.getTime().getMinute()
                             - loc1.getTime().getMinute();
                int nMinsMonth;
                switch (loc1.getDate().getMonth()) {
                    case 4: case 6: case 9: case 11:
                        nMinsMonth = 1440 * 30;
                        break;
                    case 2:
                        nMinsMonth = 1440 * 28;
                        break;
                    default:
                        nMinsMonth = 1440 * 31;
                }
                int distance = (nMinsMonth * difMonth) + (1440 * difDay)
                               + (60 * difHour) + difMin;
                if (distance >= 0 && distance < minDistance) {
                    minDistance = distance;
                    minDistLoc = loc2;
                }
            }
            // Filter movements by diff of time
            if (minDistLoc != null && minDistance <= maxMinutesInMoves &&
                    minDistance >= minMinutesInMoves) {
                ItinTime src = ItinTimeUtil.create(loc1.getDate(),
                                                   loc1.getTime(),
                                                   loc1.getGroup());
                ItinTime tgt = ItinTimeUtil.create(minDistLoc.getDate(),
                                                   minDistLoc.getTime(),
                                                   minDistLoc.getGroup());
                ItinMovement move = ItinMovementUtil.create(src, tgt);
                context.write(key, MobDataUtil.createAndWrap(move));
            }
        }
    }
}
