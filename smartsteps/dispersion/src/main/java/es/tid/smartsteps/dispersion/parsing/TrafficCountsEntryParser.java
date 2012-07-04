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

    public static final String DATE_FIELD_NAME = "date";
    public static final String CELLID_FIELD_NAME = "cellid";
    public static final String LATITUDE_FIELD_NAME = "lat";
    public static final String LONGITUDE_FIELD_NAME = "long";
    public static final String POIS_FIELD_NAME = "pois";
    
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
            entry.date = jsonObject.getString(DATE_FIELD_NAME);
            entry.cellId = jsonObject.getString(CELLID_FIELD_NAME);
            entry.latitude = jsonObject.getDouble(LATITUDE_FIELD_NAME);
            entry.longitude = jsonObject.getDouble(LONGITUDE_FIELD_NAME);
            for (String countField : entry.counts.keySet()) {
                final JSONArray parsedCounts =
                        jsonObject.getJSONArray(countField);
                ArrayList<BigDecimal> counts = entry.counts.get(countField);
                for (int i = 0; i < parsedCounts.size(); i++) {
                    counts.add(new BigDecimal(parsedCounts.getDouble(i)));
                }
            }
            JSONObject pois = jsonObject.getJSONObject(POIS_FIELD_NAME);
            for (String expectedPoi : TrafficCountsEntry.EXPECTED_POIS) {
                final JSONArray parsedCounts = pois.getJSONArray(expectedPoi);
                ArrayList<BigDecimal> counts = entry.pois.get(expectedPoi);
                for (int i = 0; i < parsedCounts.size(); i++) {
                    counts.add(new BigDecimal(parsedCounts.getDouble(i)));
                }
            }
            return entry;
        } catch (Exception ex) {
            return null;
        }
    }
}
