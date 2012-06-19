package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.GregorianCalendar;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;

/**
 * Input: <Long, ItinTime>
 * Output: <Long, ItinMovement>
 * 
 * @author dmicol
 */
public class ItinMoveClientPoisReducer extends Reducer<
        LongWritable, TypedProtobufWritable<ItinTime>,
        LongWritable, TypedProtobufWritable<ItinMovement>> {
    private static final int MINS_IN_ONE_HOUR = 60;
    private static final int HOURS_IN_ONE_DAY = 24;
    private static final int MINS_IN_ONE_DAY = MINS_IN_ONE_HOUR *
                                               HOURS_IN_ONE_DAY;
    
    private int maxMinutesInMoves;
    private int minMinutesInMoves;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.maxMinutesInMoves = conf.getItinMaxMinutesInMoves();
        this.minMinutesInMoves = conf.getItinMinMinutesInMoves();
    }

    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<ItinTime>> values,
            Context context) throws IOException, InterruptedException {
        List<ItinTime> locList = new LinkedList<ItinTime>();
        for (TypedProtobufWritable<ItinTime> value : values) {
            locList.add(value.get());
        }
        
        final GregorianCalendar calendar = new GregorianCalendar();
        for (ItinTime loc1 : locList) {
            int minDistance = Integer.MAX_VALUE;
            int minDifMonth = Integer.MAX_VALUE;
            ItinTime minDistLoc = null;
            for (ItinTime loc2 : locList) {
                if (loc2 == loc1) {
                    continue;
                }
                // TODO: migrate to Java's date classes
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
                // TODO: we must consider the year as well
                int distance = (nMinsMonth * difMonth)
                               + (MINS_IN_ONE_DAY * difDay)
                               + (MINS_IN_ONE_HOUR * difHour) + difMin;
                if (distance >= 0 && distance < minDistance) {
                    minDistance = distance;
                    minDifMonth = difMonth;
                    minDistLoc = loc2;
                }
            }
            if (minDistLoc == null ||
                    (minDistLoc.getBts() == loc1.getBts()) ||
                    (minDifMonth > 1)) {
                continue;
            }
            // Filter movements by diff of time
            if (minDistance <= this.maxMinutesInMoves &&
                    minDistance >= this.minMinutesInMoves) {
                TypedProtobufWritable<ItinMovement> move = new TypedProtobufWritable<ItinMovement>(
                        ItinMovementUtil.create(loc1, minDistLoc));
                context.write(key, move);
            }
        }
    }
}
