package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ItinMovementUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ItinMoveClientPoisReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    private final static int MAX_MINUTES_IN_MOVES = 360;
    private final static int MIN_MINUTES_IN_MOVES = 0;

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
                if (distance <= MAX_MINUTES_IN_MOVES &&
                    distance >= MIN_MINUTES_IN_MOVES)  
                {
                    ProtobufWritable<MobData> move = MobDataUtil.createAndWrap(
                            ItinMovementUtil.create(prevLoc, curLoc));
                    context.write(key, move);
                }
            }
        }
    }
}
