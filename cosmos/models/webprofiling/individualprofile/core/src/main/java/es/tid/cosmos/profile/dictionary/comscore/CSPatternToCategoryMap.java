package es.tid.cosmos.profile.dictionary.comscore;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;

/**
 * Class that maps the pattern IDs to their respective category IDs.
 *
 * @author dmicol
 */
public class CSPatternToCategoryMap {
    private static final String DELIMITER = "\t";

    private long minPatternId;
    private int size;
    private long[][] map;

    public CSPatternToCategoryMap() {
    }

    public void init(Reader input) throws IOException {
        BufferedReader br = new BufferedReader(input);
        this.minPatternId = Long.parseLong(br.readLine());
        long maxPatternId = Long.parseLong(br.readLine());
        this.size =
                this.safeLongToInt(maxPatternId - this.minPatternId + 1);
        this.map = new long[this.size][];
        String line;
        while ((line = br.readLine()) != null) {
            String[] columns = line.trim().split(DELIMITER);
            Long patternId = Long.parseLong(columns[0]);
            long[] categoryIds = new long[columns.length - 1];
            for (int i = 1; i < columns.length; i++) {
                categoryIds[i - 1] = Long.parseLong(columns[i]);
            }
            this.map[this.getIndex(patternId)] = categoryIds;
        }
        br.close();
    }

    public long[] getCategories(Long patternId) {
        long[] categories = this.map[this.getIndex(patternId)];
        if (categories == null) {
            throw new IllegalArgumentException("Unknown pattern ID: " +
                    patternId);
        }
        return categories;
    }

    private int getIndex(long patternId) {
        int index = this.safeLongToInt(patternId - this.minPatternId);
        if (index < 0 || index >= this.size) {
            throw new IllegalArgumentException("Unknown pattern ID: " +
                    patternId);
        }
        return index;
    }

    private int safeLongToInt(Long value) {
        if ((value < Integer.MIN_VALUE) || (value > Integer.MAX_VALUE)) {
            throw new IllegalArgumentException("Unable to cast value.");
        }
        return value.intValue();
    }
}
