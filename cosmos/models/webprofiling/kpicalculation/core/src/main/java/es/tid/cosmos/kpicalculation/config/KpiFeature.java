package es.tid.cosmos.kpicalculation.config;

import java.util.Arrays;

import org.apache.commons.lang.StringUtils;

/**
 * @author javierb, sortega
 */
public class KpiFeature {
    private static final int HASH_SALT = 79;
    private static final int HASH_SEED = 7;
    private final String name;
    private final String[] fields;
    private final String group;

    public KpiFeature(String name, String[] fields) {
        this(name, fields, null);
    }

    public KpiFeature(String name, String[] fields, String group) {
        this.name = name;
        this.fields = fields.clone();
        this.group = group;
    }

    public String getName() {
        return this.name;
    }

    public String[] getFields() {
        return this.fields;
    }

    public String getGroup() {
        return this.group;
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final KpiFeature other = (KpiFeature) obj;
        if ((this.name == null) ? (other.name != null)
                                : !this.name.equals(other.name)) {
            return false;
        }
        if (!Arrays.deepEquals(this.fields, other.fields)) {
            return false;
        }
        if ((this.group == null) ? (other.group != null)
                                 : !this.group.equals(other.group)) {
            return false;
        }
        return true;
    }

    @Override
    public int hashCode() {
        int hash = HASH_SEED;
        hash = HASH_SALT * hash + (this.name != null ? this.name.hashCode()
                                                     : 0);
        hash = HASH_SALT * hash + Arrays.deepHashCode(this.fields);
        hash = HASH_SALT * hash + (this.group != null ? this.group.hashCode()
                                                      : 0);
        return hash;
    }

    @Override
    public String toString() {
        return ("KpiFeature{" + "name=" + name + ", fields=[" +
                StringUtils.join(fields, ",") + "], group=" + group + '}');
    }
}
