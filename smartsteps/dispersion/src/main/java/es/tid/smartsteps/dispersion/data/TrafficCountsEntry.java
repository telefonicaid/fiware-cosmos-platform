package es.tid.smartsteps.dispersion.data;

import java.math.BigDecimal;
import java.math.MathContext;
import java.util.ArrayList;
import java.util.HashMap;

/**
 * TrafficCountsEntry
 *
 * holds all counts per sociodemographic variable in a traffic log line, plus a
 * cell ID and location information.
 *
 * @author  logc
 */
public class TrafficCountsEntry implements Entry {
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
    public String getSecondaryKey() {
        throw new UnsupportedOperationException("Secondary key is not a field" +
                "of TrafficCountsEntry");
    }

    @Override
    public BigDecimal getProportion() {
        throw new UnsupportedOperationException("Proportion is not a field of "
                + "TrafficCountsEntry");
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
}
