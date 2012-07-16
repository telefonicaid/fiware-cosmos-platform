package es.tid.cosmos.base.mapreduce;

/**
 * The ternary comosite key type for mapreduce applications.
 *
 * @author dmicol
 */
public class TernaryKey extends CompositeKey {
    private static final int CAPACITY = 3;

    private enum KeyIndex {
        PRIMARY,
        SECONDARY,
        TERTIARY
    }

    public TernaryKey() {
        super(CAPACITY);
    }

    public TernaryKey(String k1, String k2, String k3) {
        super(CAPACITY);
        this.set(KeyIndex.PRIMARY.ordinal(), k1);
        this.set(KeyIndex.SECONDARY.ordinal(), k2);
        this.set(KeyIndex.TERTIARY.ordinal(), k3);
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

    public String getTertiaryKey() {
        return this.get(KeyIndex.TERTIARY.ordinal());
    }

    public void setTertiaryKey(String key) {
        this.set(KeyIndex.TERTIARY.ordinal(), key);
    }
}
