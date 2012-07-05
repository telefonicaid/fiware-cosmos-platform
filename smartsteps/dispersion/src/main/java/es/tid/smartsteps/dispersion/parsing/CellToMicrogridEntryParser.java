package es.tid.smartsteps.dispersion.parsing;

import es.tid.smartsteps.dispersion.data.CellToMicrogridLookupEntry;

/**
 *
 * @author dmicol
 */
public class CellToMicrogridEntryParser
        extends Parser<CellToMicrogridLookupEntry> {

    public CellToMicrogridEntryParser(String delimiter) {
        super(delimiter);
    }
    
    @Override
    public CellToMicrogridLookupEntry parse(String line) {
        final String[] values = line.split(this.delimiter);
        if (values.length != 3) {
            return null;
        }
        CellToMicrogridLookupEntry entry =
                new CellToMicrogridLookupEntry(values[0], values[1],
                                               Double.parseDouble(values[2]));

        return entry;
    }
}
