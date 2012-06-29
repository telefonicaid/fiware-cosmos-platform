package es.tid.smartsteps.dispersion.parsing;

import java.math.BigDecimal;
import java.util.ArrayList;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;

/**
 * TrafficCountsEntryParser
 *
 * @author logc
 */
public class TrafficCountsEntryParser extends Parser<TrafficCountsEntry> {

    private final String[] countFields;
    
    public TrafficCountsEntryParser(String[] countFields) {
        super(null);
        this.countFields = countFields.clone();
    }
    
    @Override
    public TrafficCountsEntry parse(String value) {
        try { 
            JSONObject jsonObject = (JSONObject) JSONSerializer.toJSON(value);
            TrafficCountsEntry entry = new TrafficCountsEntry(this.countFields);
            entry.date = jsonObject.getString("date");
            entry.cellId = jsonObject.getString("cellid");
            entry.latitude = jsonObject.getDouble("lat");
            entry.longitude = jsonObject.getDouble("long");
            for (String countField : entry.counts.keySet()) {
                final JSONArray parsedCounts =
                        jsonObject.getJSONArray(countField);
                ArrayList<BigDecimal> counts = entry.counts.get(countField);
                for (int i = 0; i < parsedCounts.size(); i++) {
                    counts.add(new BigDecimal(parsedCounts.getDouble(i)));
                }
            }
            JSONObject pois = jsonObject.getJSONObject("pois");
            for (String expectedPoi : TrafficCountsEntry.EXPECTED_POIS) {
                final JSONArray parsedCounts = pois.getJSONArray(expectedPoi);
                ArrayList<BigDecimal> counts = entry.pois.get(expectedPoi);
                for (int i = 0; i < parsedCounts.size(); i++) {
                    counts.add(new BigDecimal(parsedCounts.getDouble(i)));
                }
            }
            entry.microgridId = jsonObject.optString("microgrid_id");
            entry.polygonId = jsonObject.optString("polygon_id");
            return entry;
        } catch (Exception ex) {
            return null;
        }
    }
}
