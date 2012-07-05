package es.tid.smartsteps.dispersion.data;

import java.math.MathContext;
import java.util.ArrayList;
import java.util.HashMap;

import net.sf.json.JSONObject;

import es.tid.smartsteps.dispersion.parsing.TrafficCountsEntryParser;

/**
 * TrafficCountsEntry
 *
 * holds all counts per sociodemographic variable in a traffic log line, plus a
 * cell ID and location information.
 *
 * @author  logc
 */
public class TrafficCountsEntry implements Entry {

    public static final String[] EXPECTED_POIS = { "HOME", "NONE", "WORK",
                                                   "OTHER", "BILL" };  
    private static final int HOURLY_SLOTS = 25;

    public String cellId;
    public String date;
    public double latitude;
    public double longitude;
    public HashMap<String, ArrayList<Double>> counts;
    public HashMap<String, ArrayList<Double>> pois;

    public TrafficCountsEntry(String[] countFields) {
        this.pois = new HashMap<String, ArrayList<Double>>();
        for (String expectedPoi : EXPECTED_POIS) {
            this.pois.put(expectedPoi, new ArrayList<Double>(HOURLY_SLOTS));
        }
        this.counts = new HashMap<String, ArrayList<Double>>();
        for (String countField : countFields) {
            this.counts.put(countField, new ArrayList<Double>());
        }
    }
    
    public TrafficCountsEntry(TrafficCountsEntry entry) {
        this.cellId = entry.cellId;
        this.date = entry.date;
        this.latitude = entry.latitude;
        this.longitude = entry.longitude;
        this.counts = new HashMap<String, ArrayList<Double>>();
        for (String key : entry.counts.keySet()) {
            this.counts.put(key, new ArrayList<Double>(entry.counts.get(key)));
        }
        this.pois = new HashMap<String, ArrayList<Double>>(entry.pois);
        for (String key : entry.pois.keySet()) {
            this.pois.put(key, new ArrayList<Double>(entry.pois.get(key)));
        }
    }
    
    @Override
    public String getKey() {
        return this.cellId;
    }
    
    public TrafficCountsEntry scale(double factor) {
        TrafficCountsEntry scaled = new TrafficCountsEntry(this);
        for (String countField : scaled.counts.keySet()) {
            final ArrayList<Double> countsForField = scaled.counts.get(countField);
            for (int i = 0; i < countsForField.size(); i++) {
                countsForField.set(i, countsForField.get(i) * factor);
            }
        }
        for (String poi : scaled.pois.keySet()) {
            ArrayList<Double> countsForPoi = scaled.pois.get(poi);
            for (int i = 0; i < countsForPoi.size(); i++) {
                countsForPoi.set(i, countsForPoi.get(i) * factor);
            }
        }
        return scaled;
    }
    
    public TrafficCountsEntry aggregate(TrafficCountsEntry entry) {
        TrafficCountsEntry aggregated = new TrafficCountsEntry(this);
        for (String countField : aggregated.counts.keySet()) {
            final ArrayList<Double> countsForField =
                    aggregated.counts.get(countField);
            final ArrayList<Double> entryCountsForField =
                    entry.counts.get(countField);
            if (countsForField.size() != entryCountsForField.size()) {
                throw new IllegalArgumentException(
                        "Mismatching sizes for field " + countField);
            }
            for (int i = 0; i < entryCountsForField.size(); i++) {
                countsForField.set(i,
                        countsForField.get(i) + entryCountsForField.get(i));
            }
        }
        for (String poi : aggregated.pois.keySet()) {
            final ArrayList<Double> countsForPoi = aggregated.pois.get(poi);
            final ArrayList<Double> entryCountsForPoi = aggregated.pois.get(poi);
            if (countsForPoi.size() != entryCountsForPoi.size()) {
                throw new IllegalArgumentException(
                        "Mismatching sizes for POI " + poi);
            }
            for (int i = 0; i < countsForPoi.size(); i++) {
                countsForPoi.set(i,
                                 countsForPoi.get(i) + entryCountsForPoi.get(i));
            }
        }
        
        return aggregated;
    }

    public JSONObject toJSON(){
        final JSONObject obj = new JSONObject();
        obj.put(TrafficCountsEntryParser.CELLID_FIELD_NAME, this.cellId);
        obj.put(TrafficCountsEntryParser.DATE_FIELD_NAME, this.date);
        obj.put(TrafficCountsEntryParser.LATITUDE_FIELD_NAME, this.latitude);
        obj.put(TrafficCountsEntryParser.LONGITUDE_FIELD_NAME, this.longitude);
        for (String field : this.counts.keySet()) {
            obj.put(field, this.counts.get(field));
        }
        obj.put(TrafficCountsEntryParser.POIS_FIELD_NAME, this.pois);
        return obj;
    }
}
