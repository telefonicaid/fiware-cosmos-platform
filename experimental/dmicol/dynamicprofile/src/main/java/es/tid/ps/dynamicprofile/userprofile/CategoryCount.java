package es.tid.ps.dynamicprofile.userprofile;

import org.apache.hadoop.io.Writable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

/**
 * Represents a category count.
 * 
 * @author sortega@tid.es
 */
public class CategoryCount implements Writable {

    private String category;
    private long count;

    public CategoryCount() {
        this.category = null;
        this.count = 0;
    }

    public CategoryCount(String category, long count) {
        this.category = category;
        this.count = count;
    }

    public String getCategory() {
        return category;
    }

    public void setCategory(String category) {
        this.category = category;
    }

    public long getCount() {
        return count;
    }

    public void setCount(int count) {
        this.count = count;
    }

    @Override
    public void write(DataOutput output) throws IOException {
        output.writeUTF(category);
        output.writeLong(count);
    }

    @Override
    public void readFields(DataInput input) throws IOException {
        category = input.readUTF();
        count = input.readLong();
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        final CategoryCount other = (CategoryCount) obj;
        if ((this.category == null) ? (other.category != null) : !this.category.equals(other.category))
            return false;
        if (this.count != other.count)
            return false;
        return true;
    }

    @Override
    public int hashCode() {
        int hash = 7;
        hash = 23 * hash + (this.category != null ? this.category.hashCode() : 0);
        hash = 23 * hash + (int) (this.count ^ (this.count >>> 32));
        return hash;
    }
}
