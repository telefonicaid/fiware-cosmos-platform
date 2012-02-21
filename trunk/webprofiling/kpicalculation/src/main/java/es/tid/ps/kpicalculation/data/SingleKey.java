package es.tid.ps.kpicalculation.data;

import es.tid.ps.base.mapreduce.CompositeKey;

public class SingleKey extends CompositeKey {
    private static final int CAPACITY = 1;
    private static final int PRIMARY_KEY_INDEX = 0;

    public SingleKey() {
        super(CAPACITY);
    }

    public SingleKey(String k1) {
        super(CAPACITY);
        this.set(PRIMARY_KEY_INDEX, k1);
    }

    public String getPrimaryKey() {
        return this.get(PRIMARY_KEY_INDEX);
    }

    public void setPrimaryKey(String key) {
        this.set(PRIMARY_KEY_INDEX, key);
    }
}
