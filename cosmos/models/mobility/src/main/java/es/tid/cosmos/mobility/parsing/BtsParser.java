package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.MobProtocol.Bts;

/**
 *
 * @author dmicol
 */
public class BtsParser extends Parser {
    private static final String DELIMITER = "[ ]+";
    
    public BtsParser(String line) {
        super(line, DELIMITER);
    }
    
    @Override
    public Bts parse() {
        try {
            return Bts.newBuilder()
                    .setPlaceId(this.parseInt())
                    .setComms(this.parseInt())
                    .setArea(this.parseDouble())
                    .build();
        } catch (Exception ex) {
            System.err.println(ex);
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
