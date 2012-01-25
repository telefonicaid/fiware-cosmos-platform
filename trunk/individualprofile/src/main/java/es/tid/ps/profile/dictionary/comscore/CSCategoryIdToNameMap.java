package es.tid.ps.profile.dictionary.comscore;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;

/**
 * Class that maps the category IDs to their respective names.
 * 
 * @author dmicol
 */
public class CSCategoryIdToNameMap {
    private static final String DELIMITER = "\t";
    
    private HashMap<Long, String> map;
    
    public CSCategoryIdToNameMap() {
    }
    
    public void init(InputStreamReader input) throws IOException {
        this.map = new HashMap<Long, String>();

        BufferedReader br = new BufferedReader(input);
        while (br.ready()) {
            String line = br.readLine();
            String[] columns = line.trim().split(DELIMITER);
            Long categoryId = Long.parseLong(columns[0]);
            String categoryName = columns[1];
            this.map.put(categoryId, categoryName);
        }
        br.close();
    }
    
    public String getCategoryName(long categoryId) {
        return this.map.get(categoryId);
    }
}
