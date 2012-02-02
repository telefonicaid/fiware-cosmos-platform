package es.tid.ps.base.mapreduce;

/**
 * The binary comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class BinaryKey extends CompositeKey {
    private static final int CAPACITY = 2;
    
    public BinaryKey(String k1, String k2) {
        super(CAPACITY);
        this.add(k1);
        this.add(k2);
    }
}
