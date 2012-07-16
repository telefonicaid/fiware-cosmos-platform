package es.tid.cosmos.base.mapreduce;

/**
 * The binary comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class BinaryKey extends CompositeKey {
    private static final int CAPACITY = 2;

    private enum KeyIndex {
        PRIMARY,
        SECONDARY
    }

    public BinaryKey() {
        super(CAPACITY);
    }

    public BinaryKey(String k1, String k2) {
        super(CAPACITY);
        this.set(KeyIndex.PRIMARY.ordinal(), k1);
        this.set(KeyIndex.SECONDARY.ordinal(), k2);
    }

    public String getPrimaryKey() {
        return this.get(KeyIndex.PRIMARY.ordinal());
    }

    public void setPrimaryKey(String key) {
        this.set(KeyIndex.PRIMARY.ordinal(), key);
    }

    public String getSecondaryKey() {
        return this.get(KeyIndex.SECONDARY.ordinal());
    }

    public void setSecondaryKey(String key) {
        this.set(KeyIndex.SECONDARY.ordinal(), key);
    }
}
