package es.tid.ps.dynamicprofile.userprofile;


import java.io.DataInput;
import java.io.DataOutput;
import java.io.IOException;
import java.util.*;

import org.apache.hadoop.io.Writable;

public class UserProfile implements Writable {
    private String userId;
    private Map<String, Long> counts = new HashMap<String, Long>();

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public void add(CategoryCount categoryCount) {
        Long count = counts.get(categoryCount.getCategory());
        if (count == null) {
            count = 0l;
        }
        counts.put(categoryCount.getCategory(), count + categoryCount.getCount());
    }

    @Override
    public void write(DataOutput output) throws IOException {
        ArrayList<Map.Entry<String, Long>> entries =
                new ArrayList<Map.Entry<String, Long>>(counts.entrySet());

        Collections.sort(entries, new Comparator<Map.Entry<String, Long>>() {
            @Override
            public int compare(Map.Entry<String, Long> left,
                               Map.Entry<String, Long> right) {
                return left.getValue().compareTo(right.getValue());
            }
        });

        output.writeUTF(userId);
        output.write(entries.size());
        for (Map.Entry<String, Long> entry: entries) {
            output.writeUTF(entry.getKey());
            output.writeLong(entry.getValue().longValue());
        }
    }

    @Override
    public void readFields(DataInput input) throws IOException {
        userId = input.readUTF();
        int entries = input.readInt();
        for (int i=0; i<entries; i++) {
            String category = input.readUTF();
            long count = input.readLong();
            counts.put(category, count);
        }
    }
}
