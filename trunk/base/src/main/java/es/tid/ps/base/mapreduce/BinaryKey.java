package es.tid.ps.base.mapreduce;

/**
 * The binary comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class BinaryKey extends CompositeKey {
    private static final int CAPACITY = 2;

    private static final int PRIMARY_KEY_INDEX = 0;
    private static final int SECONDARY_KEY_INDEX = 1;
    
    public BinaryKey() {
        super(CAPACITY);
    }
    
    public BinaryKey(String k1, String k2) {
        super(CAPACITY);
        this.set(PRIMARY_KEY_INDEX, k1);
        this.set(SECONDARY_KEY_INDEX, k2);
    }
    
    public String getPrimaryKey() {
        return this.get(PRIMARY_KEY_INDEX);
    }
    
    public void setPrimaryKey(String key) {
        this.set(PRIMARY_KEY_INDEX, key);
    }

    public String getSecondaryKey() {
        return this.get(SECONDARY_KEY_INDEX);
    }
    
    public void setSecondaryKey(String key) {
        this.set(SECONDARY_KEY_INDEX, key);
    }
}
