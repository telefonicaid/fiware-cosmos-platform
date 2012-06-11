package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

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
            Bts.Builder bts = Bts.newBuilder();
            bts.setPlaceId(this.parseLong());
            this.skipField();
            bts.setArea(this.parseDouble());
            bts.setComms(this.parseInt());
            return bts.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
