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
    public static final String[] COUNT_FIELDS = { "footfall_observed_basic",
                                                  "footfall_observed_female",
                                                  "footfall_observed_male",
                                                  "footfall_observed_age_0",
                                                  "footfall_observed_age_20",
                                                  "footfall_observed_age_30",
                                                  "footfall_observed_age_40",
                                                  "footfall_observed_age_50",
                                                  "footfall_observed_age_60",
                                                  "footfall_observed_age_70" };    
    private static final int HOURLY_SLOTS = 25;

    public String cellId;
    public String date;
    public long northing;
    public long easting;
    public double lat;
    public HashMap<String, ArrayList<BigDecimal>> counts;
    public ArrayList<Integer> poiFive = new ArrayList<Integer>(HOURLY_SLOTS);
    public HashMap<String, ArrayList<Integer>> pois;
    public String microgridId;
    public String polygonId;

    public TrafficCountsEntry() {
        this.pois = new HashMap<String, ArrayList<Integer>>();
        this.pois.put("HOME", new ArrayList<Integer>(HOURLY_SLOTS));
        this.pois.put("WORK", new ArrayList<Integer>(HOURLY_SLOTS));
        this.pois.put("NONE", new ArrayList<Integer>(HOURLY_SLOTS));
        this.pois.put("OTHER", new ArrayList<Integer>(HOURLY_SLOTS));
        this.pois.put("BILL", new ArrayList<Integer>(HOURLY_SLOTS));
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
        ans.add(this.northing);
        ans.add(this.easting);
        ans.add(this.lat);
        ans.add(this.counts);
        ans.add(this.poiFive);
        ans.add(this.pois);
        ans.add(this.microgridId);
        ans.add(this.polygonId);
        return ans;
    }

    public TrafficCountsEntry scale(BigDecimal factor) {
        TrafficCountsEntry scaledEntry = new TrafficCountsEntry();
        scaledEntry.date = this.date;
        scaledEntry.cellId = this.cellId;
        scaledEntry.northing = this.northing;
        scaledEntry.easting = this.easting;
        scaledEntry.lat = this.lat;
        scaledEntry.counts = this.scaleCounts(factor);
        scaledEntry.poiFive = (ArrayList<Integer>) this.poiFive.clone();
        scaledEntry.pois =
                (HashMap<String, ArrayList<Integer>>) this.pois.clone();
        scaledEntry.microgridId = this.microgridId;
        scaledEntry.polygonId = this.polygonId;
        return scaledEntry;
    }
    
    private HashMap<String, ArrayList<BigDecimal>> scaleCounts(
            BigDecimal factor) {
        HashMap<String, ArrayList<BigDecimal>> scaledCounts =
                new HashMap<String, ArrayList<BigDecimal>>();
        for (String countField : COUNT_FIELDS) {
            final ArrayList<BigDecimal> countsForField =
                    this.counts.get(countField);
            ArrayList<BigDecimal> scaledCountsForField =
                    new ArrayList<BigDecimal>();
            for (int i = 0; i < countsForField.size(); i++) {
                scaledCountsForField.add(countsForField.get(i)
                        .multiply(factor));
            }
            scaledCounts.put(countField, scaledCountsForField);
        }
        return scaledCounts;
    }
    
    public HashMap<String, ArrayList<BigDecimal>> roundCounts() {
        HashMap<String, ArrayList<BigDecimal>> roundedCounts =
                new HashMap<String, ArrayList<BigDecimal>>();
        for (String countField : COUNT_FIELDS) {
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
        obj.put("cellId", this.cellId);
        obj.put("date", this.date);
        obj.put("northing", this.northing);
        obj.put("easting", this.easting);
        obj.put("lat", this.lat);
        for (String field : TrafficCountsEntry.COUNT_FIELDS) {
            obj.put(field, this.counts.get(field));
        }
        obj.put("poi_5", this.poiFive);
        obj.put("pois", this.pois);
        obj.put("microgrid_id", this.microgridId);
        obj.put("polygon_id", this.polygonId);
        return obj;
    }
}
