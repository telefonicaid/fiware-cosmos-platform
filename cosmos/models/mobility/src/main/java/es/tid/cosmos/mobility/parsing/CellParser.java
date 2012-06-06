package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author sortega
 */
public class CellParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public CellParser(String line) {
        super(line, DELIMITER);
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
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
