package es.tid.ps.dynamicprofile.categoryextraction;

import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.ArrayList;
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

    public void readFields(DataInput in) throws IOException {
        this.deserialize(in.readUTF());
    }

    public void write(DataOutput out) throws IOException {
        out.writeUTF(this.serialize());
    }

    private String serialize() {
        StringBuilder sBuilder = new StringBuilder();
        sBuilder.delete(0, sBuilder.length());
        sBuilder.append(this.userId).append(DELIMITER);
        sBuilder.append(this.url).append(DELIMITER);
        sBuilder.append(this.count);
        for (String categoryName : this.categoryNames) {
            sBuilder.append(DELIMITER).append(categoryName);
        }
        return sBuilder.toString();
    }

    private void deserialize(String text) {
        StringTokenizer stt = new StringTokenizer(text, DELIMITER);
        try {
            this.userId = stt.nextToken();
            this.url = stt.nextToken();
            this.count = Long.parseLong(stt.nextToken());
            ArrayList<String> categoryNames = new ArrayList<String>();
            while (stt.hasMoreTokens()) {
                categoryNames.add(stt.nextToken());
            }
            this.categoryNames = (String[]) categoryNames.toArray();
        } catch (NoSuchElementException ex) {
        }
    }
}
