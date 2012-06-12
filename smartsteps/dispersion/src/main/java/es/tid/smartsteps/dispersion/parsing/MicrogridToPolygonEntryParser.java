package es.tid.smartsteps.dispersion.parsing;

import java.math.BigDecimal;

import es.tid.smartsteps.dispersion.data.MicrogridToPolygonEntry;

/**
 *
 * @author dmicol
 */
public class MicrogridToPolygonEntryParser extends Parser {
    public MicrogridToPolygonEntryParser(String delimiter) {
        super(delimiter);
    }
    
    @Override
    public MicrogridToPolygonEntry parse(String line) {
        final String[] values = line.split(this.delimiter);
        if (values.length != 3) {
            throw new IllegalArgumentException("Invalid line: " + line);
        }
        MicrogridToPolygonEntry entry = new MicrogridToPolygonEntry(values[0],
                values[1], new BigDecimal(values[2]));
        return entry;
    }

    @Override
    public MicrogridToPolygonEntry safeParse(String line) {
        try {
            return this.parse(line);
        } catch (Exception ex) {
            return null;
        }
    }
}
