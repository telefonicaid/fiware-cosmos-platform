package es.tid.ps.base.mapreduce;

/**
 * The ternary comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class TernaryKey extends CompositeKey {
    private static final int CAPACITY = 3;
    
    public TernaryKey(String k1, String k2, String k3) {
        super(CAPACITY);
        this.add(k1);
        this.add(k2);
        this.add(k3);
    }
}
