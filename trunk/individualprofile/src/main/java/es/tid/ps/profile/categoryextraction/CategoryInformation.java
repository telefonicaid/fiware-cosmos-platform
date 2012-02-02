package es.tid.ps.profile.categoryextraction;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.Arrays;
import org.apache.hadoop.io.Writable;

/*
 * Class that represents information about categories, users, and the number of
 * visits to URLs of such categories.
 *
 * @author dmicol, sortega
 */
public class CategoryInformation implements Writable {
    protected static final String DELIMITER = "\t";
    private String userId;
    private String url;
    private String date;
    private long count;
    private String[] categoryNames;

    public CategoryInformation() {
    }

    public CategoryInformation(String userId, String url, String date,
            long count, String[] categoryNames) {
        this.userId = userId;
        this.url = url;
        this.date = date;
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

    public String getDate() {
        return this.date;
    }
    
    public void setDate(String date) {
        this.date = date;
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
        this.categoryNames = categoryNames.clone();
    }

    @Override
    public void readFields(DataInput in) throws IOException {
        this.userId = in.readUTF();
        this.url = in.readUTF();
        this.date = in.readUTF();
        this.count = in.readLong();

        int categories = in.readInt();
        this.categoryNames = new String[categories];
        for (int i = 0; i < categories; i++) {
            this.categoryNames[i] = in.readUTF();
        }
    }

    @Override
    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.userId);
        out.writeUTF(this.url);
        out.writeUTF(this.date);
        out.writeLong(this.count);
        out.writeInt(this.categoryNames.length);
        for (String categoryName : this.categoryNames) {
            out.writeUTF(categoryName);
        }
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final CategoryInformation other = (CategoryInformation) obj;
        if ((this.userId == null) ? (other.userId != null)
                : !this.userId.equals(other.userId)) {
            return false;
        }
        if ((this.url == null) ? (other.url != null) : !this.url.equals(
                other.url)) {
            return false;
        }
        if ((this.date == null) ? (other.date != null) : !this.date.equals(
                other.date)) {
            return false;
        }
        if (this.count != other.count) {
            return false;
        }
        if (!Arrays.deepEquals(this.categoryNames, other.categoryNames)) {
            return false;
        }
        return true;
    }

    @Override
    public int hashCode() {
        int hash = 5;
        hash = 97 * hash + (this.userId != null ? this.userId.hashCode() : 0);
        hash = 97 * hash + (this.url != null ? this.url.hashCode() : 0);
        hash = 97 * hash + (this.date != null ? this.date.hashCode() : 0);
        hash = 97 * hash + (int) (this.count ^ (this.count >>> 32));
        hash = 97 * hash + Arrays.deepHashCode(this.categoryNames);
        return hash;
    }
}
