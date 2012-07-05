package es.tid.smartsteps.dispersion.parsing;

import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.Counts;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;
import net.sf.json.JSONSerializer;

import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 * TrafficCountsEntryParser
 *
 * @author logc
 */
public class TrafficCountsParser extends Parser<TrafficCounts> {

    public static final String DATE_FIELD_NAME = "date";
    public static final String CELLID_FIELD_NAME = "cellid";
    public static final String LATITUDE_FIELD_NAME = "lat";
    public static final String LONGITUDE_FIELD_NAME = "long";
    public static final String POIS_FIELD_NAME = "pois";
    public static final String[] EXPECTED_POIS = { "HOME", "NONE", "WORK",
                                                   "OTHER", "BILL" };
    
    private final String[] countFields;
    
    public TrafficCountsParser(String[] countFields) {
        super(null);
        this.countFields = countFields.clone();
    }
    
    @Override
    public TrafficCounts parse(String value) {
        try {
            JSONObject jsonObject = (JSONObject) JSONSerializer.toJSON(value);
            TrafficCounts.Builder builder = TrafficCounts.newBuilder();
            builder.setCellId(jsonObject.getString(CELLID_FIELD_NAME));
            builder.setDate(jsonObject.getString(DATE_FIELD_NAME));
            builder.setLatitude(jsonObject.getDouble(LATITUDE_FIELD_NAME));
            builder.setLongitude(jsonObject.getDouble(LONGITUDE_FIELD_NAME));
            builder.setCellId(jsonObject.getString(CELLID_FIELD_NAME));
            for (String countField : this.countFields) {
                final JSONArray parsedCounts =
                        jsonObject.getJSONArray(countField);
                Counts.Builder countsBuilder = Counts.newBuilder();
                countsBuilder.setName(countField);
                for (int i = 0; i < parsedCounts.size(); i++) {
                    countsBuilder.addValues(parsedCounts.getDouble(i));
                }
                builder.addFootfalls(countsBuilder);
            }
            JSONObject pois = jsonObject.getJSONObject(POIS_FIELD_NAME);
            for (String expectedPoi : EXPECTED_POIS) {
                final JSONArray parsedCounts = pois.getJSONArray(expectedPoi);
                Counts.Builder countsBuilder = Counts.newBuilder();
                countsBuilder.setName(expectedPoi);
                for (int i = 0; i < parsedCounts.size(); i++) {
                    countsBuilder.addValues(parsedCounts.getDouble(i));
                }
                builder.addPois(countsBuilder);
            }
            return builder.build();
        } catch (Exception ex) {
            return null;
        }
    }
}
