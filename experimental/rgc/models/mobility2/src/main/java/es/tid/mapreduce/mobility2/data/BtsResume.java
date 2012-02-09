package es.tid.mapreduce.mobility2.data;

import java.util.Map;

/**
 * Pojo that storages the information about one bts. The information that it
 * storages is the indentifier of hte bts, the pearson measures of his vector
 * respect the representative vectors (indentifer with their names a their
 * values) and their mobility vector
 * 
 * @author rgc
 */
public class BtsResume {
    final private String bts;
    final private Map<String, Double> pearson;
    final private Vector vector;

    /**
     * Constructor
     * 
     * @param bts
     * @param vector
     * @param pearson
     */
    public BtsResume(String bts, Vector vector, Map<String, Double> pearson) {
        this.bts = bts;
        this.vector = vector;
        this.pearson = pearson;
    }

    /**
     * @return the bts
     */
    public String getBts() {
        return bts;
    }

    /**
     * @return the pearson
     */
    public Map<String, Double> getPearson() {
        return pearson;
    }

    /**
     * @return the vector
     */
    public Vector getVector() {
        return vector;
    }
}
