package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;
import java.util.TreeMap;

import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TopCell;

/**
 * Merges a number of Catchmets with a single TopCell
 *
 * In: <[id, date], Catchments>
 * Out: <[id, date], Catchments>
 *
 * @author sortega
 */
public class TopCellAggregatorReducer extends Reducer<
        BinaryKey, TypedProtobufWritable<Catchments>,
        BinaryKey, TypedProtobufWritable<Catchments>> {

    private Map<Integer, Map <String, Double>> counts;
    private Map<String, Double> latitudes;
    private Map<String, Double> longitudes;

    @Override
    protected void setup(Context context) {
        this.counts = new TreeMap<Integer, Map<String, Double>>();
        this.latitudes = new TreeMap<String, Double>();
        this.longitudes = new TreeMap<String, Double>();
    }

    @Override
    protected void reduce(BinaryKey key,
                          Iterable<TypedProtobufWritable<Catchments>> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        this.counts.clear();
        this.latitudes.clear();
        this.longitudes.clear();

        Iterator<TypedProtobufWritable<Catchments>> iterator = values.iterator();
        Catchments firstValue = iterator.next().get();
        this.aggregateTopCell(firstValue);

        while(iterator.hasNext()) {
            this.aggregateTopCell(iterator.next().get());
        }

        context.write(key, new TypedProtobufWritable<Catchments>(
                this.buildCatchments(firstValue)));
    }

    private void aggregateTopCell(Catchments value) {
        TopCell topCell = value.getCatchments(0).getTopCells(0);
        this.latitudes.put(topCell.getId(), topCell.getLatitude());
        this.longitudes.put(topCell.getId(), topCell.getLongitude());
        int hour = value.getCatchments(0).getHour();
        Map<String, Double> catchmentMap = this.counts.get(hour);
        if (catchmentMap == null) {
            catchmentMap = new TreeMap<String, Double>();
            this.counts.put(hour, catchmentMap);
        }
        String id = topCell.getId();
        Double totalCount = catchmentMap.get(id);
        if (totalCount == null) {
            totalCount = 0d;
        }
        catchmentMap.put(id, totalCount + topCell.getCount());
    }

    private Catchments buildCatchments(Catchments prototype) {
        Catchments.Builder builder = Catchments.newBuilder(prototype)
                .clearCatchments();

        for(Entry<Integer, Map<String, Double>> catchmentEntry :
                this.counts.entrySet()) {
            Catchment.Builder catchment = Catchment.newBuilder()
                    .setHour(catchmentEntry.getKey());
            for (Entry<String, Double> topCellEntry :
                    catchmentEntry.getValue().entrySet()) {
                catchment.addTopCells(TopCell.newBuilder()
                        .setId(topCellEntry.getKey())
                        .setCount(topCellEntry.getValue())
                        .setLatitude(this.latitudes.get(topCellEntry.getKey()))
                        .setLongitude(this.longitudes.get(topCellEntry.getKey())));
            }

            builder.addCatchments(catchment);
        }

        return builder.build();
    }
}
