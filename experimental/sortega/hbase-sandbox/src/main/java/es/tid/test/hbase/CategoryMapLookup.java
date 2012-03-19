package es.tid.test.hbase;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.NavigableMap;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.util.Bytes;

/**
 *
 * @author sortega
 */
public class CategoryMapLookup {
    private final HTable profileTable;
    
    public CategoryMapLookup(HTable profileTable) {
        this.profileTable = profileTable;
    }
    
    public CategoryMap getLastFor(String userId) throws IOException {
        Result response = profileTable.get(new Get(Bytes.toBytes(userId)));
        
        Map<String, Long> counts = new HashMap<String, Long>();
        long totalCount = 0L;
        
        for (Map.Entry<byte[], NavigableMap<byte[], byte[]>> category : 
                response.getNoVersionMap().entrySet()) {
            
            String categoryName = Bytes.toString(category.getKey());
            byte[] lastDate = category.getValue().firstKey();
            long count = Bytes.toLong(category.getValue().get(lastDate));
            totalCount += count;
            
            counts.put(categoryName, count);
        }
        
        CategoryMap categories = new CategoryMap();
        for (Entry<String, Long> categoryCount : counts.entrySet()) {
            categories.put(categoryCount.getKey(), 
                    ((double) categoryCount.getValue()) / totalCount);
        }
        return categories;
    }
}
