package es.tid.ps.base.mapreduce;

/**
 * The ternary comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class TernaryKey extends CompositeKey {
    private static final int CAPACITY = 3;
    
    public TernaryKey() {
        super(CAPACITY);
    }
    
    public TernaryKey(String k1, String k2, String k3) {
        super(CAPACITY);
        this.set(0, k1);
        this.set(1, k2);
        this.set(2, k3);
    }
}
