package es.tid.smartsteps.dispersion;

import java.io.IOException;

import net.sf.json.JSONObject;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
class TrafficCountsJsonExporterReducer extends Reducer<
        Text, TypedProtobufWritable<TrafficCounts>,
        NullWritable, Text> {

    private Text outValue;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.outValue = new Text();
    }
    
    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<TrafficCounts>> values,
            Context context) throws IOException, InterruptedException {
        for (TypedProtobufWritable<TrafficCounts> value : values) {
            this.outValue.set(toJson(value.get()).toString());
            context.write(NullWritable.get(), this.outValue);
        }
    }
    
    private static JSONObject toJson(TrafficCounts counts) {
        final JSONObject obj = new JSONObject();
        obj.put(TrafficCountsParser.CELLID_FIELD_NAME, counts.getCellId());
        obj.put(TrafficCountsParser.DATE_FIELD_NAME, counts.getDate());
        obj.put(TrafficCountsParser.LATITUDE_FIELD_NAME, counts.getLatitude());
        obj.put(TrafficCountsParser.LONGITUDE_FIELD_NAME, counts.getLongitude());
        for (Counts footfallCounts : counts.getFootfallsList()) {
            obj.put(footfallCounts.getName(), footfallCounts.getValuesList());
        }
        JSONObject pois = new JSONObject();
        for (Counts poiCounts : counts.getPoisList()) {
            pois.put(poiCounts.getName(), poiCounts.getValuesList());
        }
        obj.put(TrafficCountsParser.POIS_FIELD_NAME, pois);
        return obj;
    }
}
