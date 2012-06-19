package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Itinerary;

/**
 * Input: <ItinRange, ClusterVector>
 * Output: <Long, Itinerary>
 * 
 * @author dmicol
 */
class ItinGetItineraryReducer extends Reducer<
        ProtobufWritable<ItinRange>, TypedProtobufWritable<ClusterVector>,
        LongWritable, TypedProtobufWritable<Itinerary>> {
    private double percAbsoluteMax;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.percAbsoluteMax = conf.getItinPercAbsoluteMax();
    }
    
    @Override
    protected void reduce(ProtobufWritable<ItinRange> key,
            Iterable<TypedProtobufWritable<ClusterVector>> values,
            Context context) throws IOException, InterruptedException {
        key.setConverter(ItinRange.class);
        final ItinRange moveRange = key.get();
        double absMax = Double.MIN_VALUE;
        for (TypedProtobufWritable<ClusterVector> value : values) {
            final ClusterVector distMoves = value.get();
            ClusterVector.Builder peaksMoves = ClusterVector.newBuilder();
            // Vector normalization
            for (int j = 0; j < distMoves.getComsCount(); j++) {
                double elem;
                if (j >= 72 && j <= 95) {
                    // 25 Thursdays in the period
                    elem = distMoves.getComs(j) / 25.0D;
                } else {
                    // 26 Mondays, Tuesdays, Wednesdays, Fridays, Saturdays and
                    // Sundays in the period
                    elem = distMoves.getComs(j) / 26.0D;
                }
                peaksMoves.addComs(elem);
                // Get absolute maximum
                if (elem > absMax) {
                    absMax = elem;
                }
            }
            // Filter small movements
            for (int j = 0; j < peaksMoves.getComsCount(); j++) {
                if (peaksMoves.getComs(j) < absMax * percAbsoluteMax / 100.0D) {
                    peaksMoves.setComs(j, 0.0D);
                }
            }
            // Extract itineraries
            Itinerary.Builder itin = Itinerary.newBuilder();
            itin.setSource(moveRange.getPoiSrc());
            itin.setTarget(moveRange.getPoiTgt());
            for (int j = 0; j < peaksMoves.getComsCount(); j++) {
                // Zone starts
                if (peaksMoves.getComs(j) != 0.0D) {
                    double peak = 0.0D;
                    itin.setWdayInit(j);
                    itin.setRangeInit(j);
                    for (; j < peaksMoves.getComsCount()
                            && peaksMoves.getComs(j) != 0.0D; j++) {
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
                    Itinerary.Builder itinFinal = itin.clone();
                    context.write(new LongWritable(moveRange.getNode()),
                                  new TypedProtobufWritable<Itinerary>(
                                          itinFinal.build()));
                }
            }
        }
    }
}
