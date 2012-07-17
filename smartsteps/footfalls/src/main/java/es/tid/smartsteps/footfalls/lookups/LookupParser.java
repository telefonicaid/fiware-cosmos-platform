package es.tid.smartsteps.footfalls.lookups;

import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;
import es.tid.smartsteps.footfalls.parsing.Parser;
import es.tid.smartsteps.footfalls.parsing.ParserUtil;

/**
 *
 * @author dmicol, sortega
 */
public class LookupParser extends Parser<Lookup> {

    public LookupParser(String delimiter) {
        super(delimiter);
    }

    @Override
    public Lookup parse(String line) {
        final String[] values = line.split(this.delimiter);
        if (values.length != 3) {
            return null;
        }
        return Lookup.newBuilder()
                .setKey(ParserUtil.safeUnquote(values[0]))
                .setValue(ParserUtil.safeUnquote(values[1]))
                .setProportion(Double.parseDouble(values[2]))
                .build();
    }
}
