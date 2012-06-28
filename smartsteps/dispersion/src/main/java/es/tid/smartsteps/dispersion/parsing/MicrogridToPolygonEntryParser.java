package es.tid.smartsteps.dispersion.parsing;

import java.math.BigDecimal;

import org.apache.commons.validator.routines.BigDecimalValidator;

import es.tid.smartsteps.dispersion.data.MicrogridToPolygonLookupEntry;

/**
 *
 * @author dmicol
 */
public class MicrogridToPolygonEntryParser
        extends Parser<MicrogridToPolygonLookupEntry> {

    public MicrogridToPolygonEntryParser(String delimiter) {
        super(delimiter);
    }
    
    @Override
    public MicrogridToPolygonLookupEntry parse(String line) {
        final String[] values = line.split(this.delimiter);
        if (values.length != 3) {
            return null;
        }
        if (!BigDecimalValidator.getInstance().isValid(values[2])) {
            return null;
        }
        MicrogridToPolygonLookupEntry entry = new MicrogridToPolygonLookupEntry(
                values[0], values[1], new BigDecimal(values[2]));
        return entry;
    }
}
