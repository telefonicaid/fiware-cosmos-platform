package es.tid.cosmos.mobility.aggregatedmatrix.group;

import java.io.IOException;
import java.util.GregorianCalendar;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.ItinTimeUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixTime;

/**
 * Input: <Long, MatrixTime>
 * Output: <Long, ItinMovement>
 *
 * @author dmicol
 */
class MatrixMoveClientReducer extends Reducer<LongWritable,
        TypedProtobufWritable<MatrixTime>, LongWritable, TypedProtobufWritable<ItinMovement>> {
    private static final int MINS_IN_ONE_HOUR = 60;
    private static final int HOURS_IN_ONE_DAY = 24;
    private static final int MINS_IN_ONE_DAY = MINS_IN_ONE_HOUR *
                                               HOURS_IN_ONE_DAY;
    
    private int maxMinutesInMoves;
    private int minMinutesInMoves;
    private boolean includeIntraMoves;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.maxMinutesInMoves = conf.getInt(Config.MTX_MAX_MINUTES_IN_MOVES,
                                             Integer.MAX_VALUE);
        this.minMinutesInMoves = conf.getInt(Config.MTX_MIN_MINUTES_IN_MOVES,
                                             Integer.MIN_VALUE);
        this.includeIntraMoves = conf.getBoolean(Config.MTX_INCLUDE_INTRA_MOVES,
                                                 false);
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<MatrixTime>> values,
            Context context) throws IOException, InterruptedException {
        List<MatrixTime> locList = new LinkedList<MatrixTime>();
        for (TypedProtobufWritable<MatrixTime> value : values) {
            locList.add(value.get());
        }

        final GregorianCalendar calendar = new GregorianCalendar();
        for (MatrixTime loc1 : locList) {
            int minDistance = Integer.MAX_VALUE;
            int minDifMonth = Integer.MAX_VALUE;
            MatrixTime minDistLoc = null;
            for (MatrixTime loc2 : locList) {
                if (loc2 == loc1) {
                    continue;
                }
                int difMonth = loc2.getDate().getMonth()
                               - loc1.getDate().getMonth();
                int difDay = loc2.getDate().getDay()
                             - loc1.getDate().getDay();
                int difHour = loc2.getTime().getHour()
                              - loc1.getTime().getHour();
                int difMin = loc2.getTime().getMinute()
                             - loc1.getTime().getMinute();
                int nMinsMonth;
                switch (loc1.getDate().getMonth()) {
                    case 4: case 6: case 9: case 11:
                        nMinsMonth = MINS_IN_ONE_DAY * 30;
                        break;
                    case 2:
                        if (calendar.isLeapYear(loc1.getDate().getYear())) {
                            nMinsMonth = MINS_IN_ONE_DAY * 29;
                        } else {
                            nMinsMonth = MINS_IN_ONE_DAY * 28;
                        }
                        break;
                    default:
                        nMinsMonth = MINS_IN_ONE_DAY * 31;
                }
                int distance = (nMinsMonth * difMonth)
                               + (MINS_IN_ONE_DAY * difDay)
                               + (MINS_IN_ONE_HOUR * difHour) + difMin;
                if (distance >= 0 && distance < minDistance) {
                    minDistance = distance;
                    minDifMonth = difMonth;
                    minDistLoc = loc2;
                }
            }
            if (minDistLoc == null || minDifMonth > 1 ||
                    (minDistLoc.getGroup() == loc1.getGroup() &&
                        minDistLoc.getBts() == loc1.getBts()) ||
                    (!this.includeIntraMoves &&
                            minDistLoc.getGroup() == loc1.getGroup())) {
                continue;
            }
            // Filter movements by diff of time
            if (minDistance <= this.maxMinutesInMoves &&
                    minDistance >= this.minMinutesInMoves) {
                ItinTime src = ItinTimeUtil.create(loc1.getDate(),
                                                   loc1.getTime(),
                                                   loc1.getGroup());
                ItinTime tgt = ItinTimeUtil.create(minDistLoc.getDate(),
                                                   minDistLoc.getTime(),
                                                   minDistLoc.getGroup());
                ItinMovement move = ItinMovementUtil.create(src, tgt);
                context.write(key, new TypedProtobufWritable<ItinMovement>(move));
            }
        }
    }
}
