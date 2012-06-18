package es.tid.smartsteps.dispersion.data;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import es.tid.smartsteps.dispersion.parsing.Parser;

/**
 *
 * @author dmicol
 */
public class LookupTable {
    private Map<String, List<Entry>> entries;
    
    public LookupTable() {
        this.entries = new HashMap<String, List<Entry>>();
    }
    
    public void load(Reader reader, Parser parser) throws IOException {
        BufferedReader br = new BufferedReader(reader);
        String line;
        while ((line = br.readLine()) != null) {
            this.add(parser.parse(line));
        }
    }
    
    public void add(Entry entry) {
        List<Entry> entryList;
        if (this.entries.containsKey(entry.getKey())) {
            entryList = this.entries.get(entry.getKey());
        } else {
            entryList = new LinkedList<Entry>();
            this.entries.put(entry.getKey(), entryList);
        }
        entryList.add(entry);
    }
    
    public List<Entry> get(String key) {
        return this.entries.get(key);
    }
}
