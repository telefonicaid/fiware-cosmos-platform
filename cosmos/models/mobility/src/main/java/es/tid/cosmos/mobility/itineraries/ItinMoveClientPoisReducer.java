package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, ItinTime>
 * Output: <Long, ItinMovement>
 * 
 * @author dmicol
 */
public class ItinMoveClientPoisReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
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
        ItinTime prevLoc;
        ItinTime curLoc = null;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            MobData mobData = value.get();
            prevLoc = curLoc;
            curLoc = mobData.getItinTime();
            if (prevLoc == null) {
                // We are analyzing the first record, so move on to the next one
                continue;
            }
            
            if (curLoc.getBts() != prevLoc.getBts()) {
                int difMonth = curLoc.getDate().getMonth()
                               - prevLoc.getDate().getMonth();
                if (difMonth > 1) {
                    continue;
                }
                int difDay = curLoc.getDate().getDay()
                             - prevLoc.getDate().getDay();
                int difHour = curLoc.getTime().getHour()
                              - prevLoc.getTime().getHour();
                int difMin = curLoc.getTime().getMinute()
                             - prevLoc.getTime().getMinute();
                int nMinsMonth;
                switch (prevLoc.getDate().getMonth()) {
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
                // Filter movements by diff of time
                if (distance <= maxMinutesInMoves &&
                        distance >= minMinutesInMoves) {
                    ProtobufWritable<MobData> move = MobDataUtil.createAndWrap(
                            ItinMovementUtil.create(prevLoc, curLoc));
                    context.write(key, move);
                }
            }
        }
    }
}
