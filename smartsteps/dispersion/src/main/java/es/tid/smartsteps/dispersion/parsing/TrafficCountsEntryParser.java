package es.tid.smartsteps.dispersion.parsing;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashMap;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import es.tid.smartsteps.dispersion.data.TrafficCountsEntry;

/**
 * TrafficCountsEntryParser
 *
 * @author logc
 */
public class TrafficCountsEntryParser extends Parser {
    public TrafficCountsEntryParser() {
        super(null);
    }
    
    @Override
    public TrafficCountsEntry parse(String value) {
        JSONObject jsonObject = (JSONObject) JSONSerializer.toJSON(value);
        TrafficCountsEntry entry = new TrafficCountsEntry();
        entry.date = jsonObject.getString("date");
        entry.cellId = jsonObject.getString("cellid");
        entry.northing = jsonObject.getLong("northing");
        entry.easting = jsonObject.getLong("easting");
        entry.lat = jsonObject.getDouble("lat");
        entry.counts = new HashMap<String, ArrayList<BigDecimal>>();
        for (String countField : TrafficCountsEntry.COUNT_FIELDS) {
            final JSONArray parsedCounts = jsonObject.getJSONArray(countField);
            ArrayList<BigDecimal> counts = new ArrayList<BigDecimal>();
            for (int i = 0; i < parsedCounts.size(); i++) {
                counts.add(new BigDecimal(parsedCounts.getDouble(i)));
            }
            entry.counts.put(countField, counts);
        }
        entry.poiFive = new ArrayList<Integer>(
                jsonObject.getJSONArray("poi_5"));
        JSONObject pois = jsonObject.getJSONObject("pois");
        for (String expectedPoi : TrafficCountsEntry.EXPECTED_POIS) {
            entry.pois.put(expectedPoi, new ArrayList<Integer>(
                    pois.getJSONArray(expectedPoi)));
        }
        entry.microgridId = jsonObject.optString("microgrid_id");
        entry.polygonId = jsonObject.optString("polygon_id");
        return entry;
    }

    @Override
    public TrafficCountsEntry safeParse(String line) {
        try {
            return this.parse(line);
        } catch (Exception ex) {
            return null;
        }
    }
}
