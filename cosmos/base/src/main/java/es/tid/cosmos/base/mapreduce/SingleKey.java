package es.tid.cosmos.base.mapreduce;

/**
 * The single key type for mapreduce applications.
 *
 * @author javierb
 */
public class SingleKey extends CompositeKey {
    private static final int CAPACITY = 1;

    private enum KeyIndex {
        PRIMARY
    }

    public SingleKey() {
        super(CAPACITY);
    }

    public SingleKey(String k1) {
        super(CAPACITY);
        this.set(KeyIndex.PRIMARY.ordinal(), k1);
    }

    public String getKey() {
        return this.get(KeyIndex.PRIMARY.ordinal());
    }

    public void setKey(String key) {
        this.set(KeyIndex.PRIMARY.ordinal(), key);
    }
}
