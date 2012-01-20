package es.tid.ps.dynamicprofile.categoryextraction;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.NoSuchElementException;
import java.util.StringTokenizer;

import org.apache.hadoop.io.Writable;

public class CategoryInformation implements Writable {
    protected final static String DELIMITER = "\t";

    private String userId;
    private String url;
    private long count;
    private String[] categoryNames;

    public CategoryInformation() {
    }

    public CategoryInformation(String userId, String url, long count,
            String[] categoryNames) {
        this.userId = userId;
        this.url = url;
        this.count = count;
        this.categoryNames = categoryNames.clone();
    }

    public String getUserId() {
        return this.userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getUrl() {
        return this.url;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public long getCount() {
        return this.count;
    }

    public void setCount(long count) {
        this.count = count;
    }

    public String[] getCategoryNames() {
        return this.categoryNames;
    }

    public void setCategoryNames(String[] categoryNames) {
        this.categoryNames = categoryNames;
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        this.userId = in.readUTF();
        this.url = in.readUTF();
        this.count = in.readLong();

        int categories = in.readInt();
        this.categoryNames = new String[categories];
        for (int i=0; i<categories; i++) {
            this.categoryNames[i] = in.readUTF();
        }
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.userId);
        out.writeUTF(this.url);
        out.writeLong(this.count);

        out.writeInt(this.categoryNames.length);
        for (String categoryName : this.categoryNames) {
            out.writeUTF(categoryName);
        }
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        final CategoryInformation other = (CategoryInformation) obj;
        if ((this.userId == null) ? (other.userId != null) : !this.userId.equals(other.userId))
            return false;
        if ((this.url == null) ? (other.url != null) : !this.url.equals(other.url))
            return false;
        if (this.count != other.count)
            return false;
        if (!Arrays.deepEquals(this.categoryNames, other.categoryNames))
            return false;
        return true;
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 97 * hash + (this.userId != null ? this.userId.hashCode() : 0);
        hash = 97 * hash + (this.url != null ? this.url.hashCode() : 0);
        hash = 97 * hash + (int) (this.count ^ (this.count >>> 32));
        hash = 97 * hash + Arrays.deepHashCode(this.categoryNames);
        return hash;
    }
}
