package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author sortega
 */
public class CellParser extends Parser {
    public CellParser(String line, String separator) {
        super(line, separator);
    }

    @Override
    public Cell parse() {
        try {
            return Cell.newBuilder()
                .setCellId(parseCellId())
                .setBts(parseLong())
                .setGeoloc1(parseInt())
                .setGeoloc2(parseInt())
                .setPosx(parseInt())
                .setPosy(parseInt())
                .build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
