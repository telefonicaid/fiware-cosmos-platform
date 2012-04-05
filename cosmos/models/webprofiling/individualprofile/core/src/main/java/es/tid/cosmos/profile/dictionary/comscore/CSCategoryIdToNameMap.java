package es.tid.cosmos.profile.dictionary.comscore;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

/**
 * Class that maps the category IDs to their respective names.
 * 
 * @author dmicol
 */
public class CSCategoryIdToNameMap {
    private static final String DELIMITER = "\t";
    
    private Map<Long, String> map;
    
    public CSCategoryIdToNameMap() {
    }
    
    public void init(Reader input) throws IOException {
        this.map = new HashMap<Long, String>();

        BufferedReader br = new BufferedReader(input);
        String line;
        while ((line = br.readLine()) != null) {
            String[] columns = line.trim().split(DELIMITER);
            Long categoryId = Long.parseLong(columns[0]);
            String categoryName = columns[1];
            this.map.put(categoryId, categoryName);
        }
        br.close();
    }
    
    public Collection<String> getCategories() {
        return this.map.values();
    }
    
    public String getCategoryName(long categoryId) {
        if (!this.map.containsKey(categoryId)) {
            throw new IllegalArgumentException("Unknown category ID: " +
                    categoryId);
        }
        return this.map.get(categoryId);
    }
}
