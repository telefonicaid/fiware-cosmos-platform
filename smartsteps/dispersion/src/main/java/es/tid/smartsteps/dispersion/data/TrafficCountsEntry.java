package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;
import java.math.MathContext;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import net.sf.json.JSONObject;

/**
 * TrafficCountsEntry
 *
 * holds all counts per sociodemographic variable in a traffic log line, plus a
 * cell ID and location information.
 *
 * @author  logc
 */
public class TrafficCountsEntry implements FieldsEntry {

    public static final String[] EXPECTED_POIS = {"HOME", "NONE", "WORK",
                                                  "OTHER", "BILL"};  
    private static final int HOURLY_SLOTS = 25;

    public String cellId;
    public String date;
    public double latitude;
    public double longitude;
    public HashMap<String, ArrayList<BigDecimal>> counts;
    public HashMap<String, ArrayList<BigDecimal>> pois;
    public String microgridId;
    public String polygonId;

    public TrafficCountsEntry(String[] countFields) {
        this.pois = new HashMap<String, ArrayList<BigDecimal>>();
        this.pois.put("HOME", new ArrayList<BigDecimal>(HOURLY_SLOTS));
        this.pois.put("WORK", new ArrayList<BigDecimal>(HOURLY_SLOTS));
        this.pois.put("NONE", new ArrayList<BigDecimal>(HOURLY_SLOTS));
        this.pois.put("OTHER", new ArrayList<BigDecimal>(HOURLY_SLOTS));
        this.pois.put("BILL", new ArrayList<BigDecimal>(HOURLY_SLOTS));
        this.counts = new HashMap<String, ArrayList<BigDecimal>>();
        for (String countField : countFields) {
            this.counts.put(countField, new ArrayList<BigDecimal>());
        }
    }
    
    public TrafficCountsEntry(TrafficCountsEntry entry) {
        this.cellId = entry.cellId;
        this.date = entry.date;
        this.latitude = entry.latitude;
        this.longitude = entry.longitude;
        this.counts = new HashMap<String, ArrayList<BigDecimal>>();
        for (String key : entry.counts.keySet()) {
            this.counts.put(key, new ArrayList<BigDecimal>(entry.counts.get(key)));
        }
        this.pois = new HashMap<String, ArrayList<BigDecimal>>(entry.pois);
        for (String key : entry.pois.keySet()) {
            this.pois.put(key, new ArrayList<BigDecimal>(entry.pois.get(key)));
        }
        this.microgridId = entry.microgridId;
        this.polygonId = entry.polygonId;
    }
    
    @Override
    public String getKey() {
        if (this.polygonId != null && !this.polygonId.isEmpty()) {
            return this.polygonId;
        } else if (this.microgridId != null && !this.microgridId.isEmpty()) {
            return this.microgridId;
        } else {
            return this.cellId;
        }
    }

    @Override
    public List<Object> getFields() {
        ArrayList<Object> ans = new ArrayList<Object>();
        ans.add(this.date);
        ans.add(this.latitude);
        ans.add(this.longitude);
        ans.add(this.counts);
        ans.add(this.pois);
        ans.add(this.microgridId);
        ans.add(this.polygonId);
        return ans;
    }
    
    public TrafficCountsEntry scale(BigDecimal factor) {
        TrafficCountsEntry scaled = new TrafficCountsEntry(this);
        for (String countField : scaled.counts.keySet()) {
            final ArrayList<BigDecimal> countsForField =
                    scaled.counts.get(countField);
            for (int i = 0; i < countsForField.size(); i++) {
                countsForField.set(i, countsForField.get(i).multiply(factor));
            }
        }
        for (String poi : scaled.pois.keySet()) {
            ArrayList<BigDecimal> countsForPoi = scaled.pois.get(poi);
            for (int i = 0; i < countsForPoi.size(); i++) {
                countsForPoi.set(i, countsForPoi.get(i).multiply(factor));
            }
        }
        
        return scaled;
    }
    
    public HashMap<String, ArrayList<BigDecimal>> roundCounts() {
        HashMap<String, ArrayList<BigDecimal>> roundedCounts =
                new HashMap<String, ArrayList<BigDecimal>>();
        for (String countField : this.counts.keySet()) {
            final ArrayList<BigDecimal> countsForField =
                    this.counts.get(countField);
            ArrayList<BigDecimal> roundedCountsForField =
                    new ArrayList<BigDecimal>();
            for (int i = 0; i < roundedCounts.size(); i++) {
                roundedCountsForField.add(i, countsForField.get(i).round(
                        MathContext.UNLIMITED));
            }
            roundedCounts.put(countField, roundedCountsForField);
        }
        return roundedCounts;
    }

    public JSONObject toJSON(){
        final JSONObject obj = new JSONObject();
        obj.put("cellid", this.cellId);
        obj.put("date", this.date);
        obj.put("lat", this.latitude);
        obj.put("long", this.longitude);
        for (String field : this.counts.keySet()) {
            obj.put(field, this.counts.get(field));
        }
        obj.put("pois", this.pois);
        obj.put("microgrid_id", this.microgridId);
        obj.put("polygon_id", this.polygonId);
        return obj;
    }
}
