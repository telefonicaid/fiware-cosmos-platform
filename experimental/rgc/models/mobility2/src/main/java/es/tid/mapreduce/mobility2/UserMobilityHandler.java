package es.tid.mapreduce.mobility2;

import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import es.tid.mapreduce.mobility2.data.BtsResume;
import es.tid.mapreduce.mobility2.data.RepresentVector;
import es.tid.mapreduce.mobility2.data.UserMobilityData;
import es.tid.mapreduce.mobility2.data.Vector;

/**
 * This class proccesses all the entries of the a unique user.
 * 
 * The input is the user identifier in the key in format Text, and a list with
 * all the CDRs in the values.
 * 
 * First preparare the data, filtering and discarting all the CDRs that we don´t
 * know their bts, in this process count the number of cdrs a disscard the users
 * is not between a minimum and a maximun (by default 0 and 5000). The
 * preparation of this data aggretate the data of cdrs by bts.
 * 
 * On the second phase we choose the pdis from the calls in the bts, this is
 * ckecking by two conditions, that the number of calls are greater than a
 * percentage and the absolute number are greater than define value (by default
 * 5% and 12)
 * 
 * On the third phase processes the data. It chooses what is a PDI and compare
 * each PDI with the representatives vectors through the Pearson Measure.
 * 
 * 
 * @author rgc
 */
public class UserMobilityHandler {
    final static int MAX_NUM_CDR = 5000;
    final static int MIN_NUM_CALL_PDI = 12;
    final static int MIN_PER_CALL_PDI = 5;

    /**
     * Structure in where we load the data of the representative vectors
     */
    private final List<RepresentVector> representVectors;

    /**
     * percentage that marks the threshold to for considering that the bts has
     * enough CDRS to became a PDI
     */
    private int threshold;
    /**
     * structure with the data of user for processing
     */
    private final Map<String, List<Calendar>> userData;

    public UserMobilityHandler(List<RepresentVector> representVectors) {
        this.representVectors = representVectors;
        this.userData = new HashMap<String, List<Calendar>>();

    }

    /**
     * This method process the data of the user,filtering and discarting all the
     * CDRs, then chooses the PDI, calculate the Pearson Measure respect the
     * referencial vectors and then return this result
     * 
     * @param values
     *            are all the values aggregated during the mapping phase, these
     *            values contain the Cdrs of the user
     * @return a structure with the PDIs their mobility vector a the Pearson
     *         Measure
     * @throws RuntimeException
     *             if the number of CDRs is upper than a limit TODO rgc change
     *             this runtimException for another define exception
     */
    public List<BtsResume> processUser(Iterable<UserMobilityData> values) {
        this.userData.clear();
        int count = 0;
        for (UserMobilityData umd : values) {
            if (++count > MAX_NUM_CDR) {
                throw new RuntimeException();
            }
            if (umd.getBts().isEmpty()) {
                continue;
            }
            String bts = umd.getBts();
            Calendar date = umd.getDateCall();
            if (this.userData.containsKey(bts)) {
                this.userData.get(bts).add(date);
            } else {
                List<Calendar> list = new ArrayList<Calendar>();
                list.add(date);
                this.userData.put(umd.getBts(), list);
            }
        }
        calculatePercentage(count);
        return processDataByDate();
    }

    /**
     * Method that calculate the percentage that marks the threshold to for
     * considering that the bts has enough CDRS to became a PDI
     * 
     * @param num
     *            total entries lines of cdrs
     */
    private void calculatePercentage(int num) {
        this.threshold = (int) Math
                .floor((((double) num * MIN_PER_CALL_PDI ) / 100.0));
    }

    /**
     * This method processes the data. It chooses what is a PDI from the
     * threshold, and the compare each PDI with the representatives vectors
     * through the Pearson Measure0
     * 
     * TODO(rgc) change the implementation the entry to a new pojo
     */
    private List<BtsResume> processDataByDate() {
        List<Entry<String, List<Calendar>>> pdis = new ArrayList<Entry<String, List<Calendar>>>();
        for (Entry<String, List<Calendar>> entry : this.userData.entrySet()) {
            int size = entry.getValue().size();
            if (size > threshold && size > MIN_NUM_CALL_PDI) {
                pdis.add(entry);
            }
        }

        List<BtsResume> resume = new ArrayList<BtsResume>();
        for (Entry<String, List<Calendar>> entry : pdis) {
            Vector vector = Vector.generateVector(entry.getValue());
            vector.preNormalizeByMobility();
            vector.normalize();

            Map<String, Double> pearson = calculatePearonsFromReprents(vector);
            resume.add(new BtsResume(entry.getKey(), vector, pearson));
        }
        return resume;
    }

    /**
     * This method calculates the Pearson Measure from one vector mobility
     * respect the representative vectors
     * 
     * @param vector
     *            the vector movility of one user in a tds
     * @return a structure with all the Pearson Measure respect the all
     *         representative vectors
     */
    private Map<String, Double> calculatePearonsFromReprents(Vector vector) {
        Map<String, Double> output = new HashMap<String, Double>();
        for (RepresentVector represent : representVectors) {
            double value = vector.getPearsonCorrelation(represent.getVector());
            output.put(represent.getName(), value);
        }
        return output;
    }
}
