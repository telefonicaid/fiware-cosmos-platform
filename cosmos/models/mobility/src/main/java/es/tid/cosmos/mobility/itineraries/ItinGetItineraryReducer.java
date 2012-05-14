package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <ItinRange, ClusterVector>
 * Output: <Long, Itinerary>
 * 
 * @author dmicol
 */
public class ItinGetItineraryReducer extends Reducer<
        ProtobufWritable<ItinRange>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    private static final double PERC_ABSOLUTE_MAX = 20.0D;

    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(ItinRange.class);
        final ItinRange moveRange = key.get();
        double absMax = 0.0D;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final ClusterVector distMoves = mobData.getClusterVector();
            ClusterVector.Builder peaksMoves = ClusterVector.newBuilder();
            // Vector normalization
            for (int j = 0; j < distMoves.getComsCount(); j++) {
                if (j >= 24 && j <= 95) {
                    double elem;
                    // 30 Tuesdays, Wednesdays and Thursdays in the period
                    if (j < 72 || j > 95) {
                        elem = distMoves.getComs(j) / 30.0D;
                    } else {
                        // 31 Mondays, Fridays, Saturdays and Sundays in the
                        // period
                        elem = distMoves.getComs(j) / 31.0D;
                    }
                    peaksMoves.addComs(elem);
                    // Get absolute maximum
                    if (elem > absMax) {
                        absMax = elem;
                    }
                }
            }
            // Filter small movements
            for (int j = 0; j < peaksMoves.getComsCount(); j++) {
                if (peaksMoves.getComs(j) <
                        (absMax * PERC_ABSOLUTE_MAX / 100.0)) {
                    peaksMoves.setComs(j, 0.0D);
                }
            }
            // Extract itineraries
            Itinerary.Builder itin = Itinerary.newBuilder();
            itin.setSource(moveRange.getPoiSrc());
            itin.setTarget(moveRange.getPoiTgt());
            for (int j = 0; j < peaksMoves.getComsCount(); j++) {
                if (peaksMoves.getComs(j) != 0.0D) // Zone starts
                {
                    double peak = 0.0D;
                    itin.setWdayInit(j);
                    itin.setRangeInit(j);
                    for (; j < peaksMoves.getComsCount()
                            && peaksMoves.getComs(j) != 0; j++) {
                        if (peaksMoves.getComs(j) > peak) {
                            // Peak starts
                            peak = peaksMoves.getComs(j);
                            itin.setWdayPeakInit(j);
                            itin.setRangePeakInit(j);
                            itin.setWdayPeakFin(j);
                            itin.setRangePeakFin(j);
                            for (; j < peaksMoves.getComsCount() - 1
                                    && peaksMoves.getComs(j)
                                    == peaksMoves.getComs(j + 1);
                                    j++) {
                                itin.setWdayPeakFin(j + 1);
                                itin.setRangePeakFin(j + 1);
                            }
                        }
                        itin.setWdayFin(j);
                        itin.setRangeFin(j);
                    }
                    itin.setWdayInit((itin.getWdayInit() / 24 + 1) % 7);
                    itin.setWdayPeakInit((itin.getWdayPeakInit() / 24 + 1) % 7);
                    itin.setWdayPeakFin((itin.getWdayPeakFin() / 24 + 1) % 7);
                    itin.setWdayFin((itin.getWdayFin() / 24 + 1) % 7);
                    itin.setRangeInit(itin.getRangeInit() % 24);
                    itin.setRangePeakInit(itin.getRangePeakInit() % 24);
                    itin.setRangePeakFin(itin.getRangePeakFin() % 24);
                    itin.setRangeFin(itin.getRangeFin() % 24);
                    context.write(new LongWritable(moveRange.getNode()),
                                  MobDataUtil.createAndWrap(itin.build()));
                }
            }
        }
    }
}
