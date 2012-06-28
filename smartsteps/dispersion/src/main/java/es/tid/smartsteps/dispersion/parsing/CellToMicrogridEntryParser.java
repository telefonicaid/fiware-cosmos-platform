package es.tid.smartsteps.dispersion.parsing;

import java.math.BigDecimal;

import org.apache.commons.validator.routines.BigDecimalValidator;

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
        if (!BigDecimalValidator.getInstance().isValid(values[2])) {
            return null;
        }
        CellToMicrogridLookupEntry entry =
                new CellToMicrogridLookupEntry(values[0], values[1],
                                               new BigDecimal(values[2]));

        return entry;
    }
}
