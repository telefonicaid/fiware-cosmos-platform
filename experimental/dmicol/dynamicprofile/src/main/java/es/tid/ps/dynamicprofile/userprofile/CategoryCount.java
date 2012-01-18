package es.tid.ps.dynamicprofile.userprofile;

import org.apache.hadoop.io.Writable;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;

public class CategoryCount implements Writable {

    private String category;
    private long count;

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
}
