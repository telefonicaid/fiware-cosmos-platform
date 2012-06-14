package es.tid.smartsteps.dispersion.parsing;

import java.math.BigDecimal;

import es.tid.smartsteps.dispersion.data.CellToMicrogridEntry;

/**
 *
 * @author dmicol
 */
public class CellToMicrogridEntryParser extends Parser {
    public CellToMicrogridEntryParser(String delimiter) {
        super(delimiter);
    }
    
    @Override
    public CellToMicrogridEntry parse(String line) {
        final String[] values = line.split(this.delimiter);
        if (values.length != 3) {
            throw new IllegalArgumentException("Invalid line: " + line);
        }
        CellToMicrogridEntry entry = new CellToMicrogridEntry(values[0],
                values[1], new BigDecimal(values[2]));
        return entry;
    }
    
    @Override
    public CellToMicrogridEntry safeParse(String line) {
        try {
            return this.parse(line);
        } catch (Exception ex) {
            return null;
        }
    }
}
