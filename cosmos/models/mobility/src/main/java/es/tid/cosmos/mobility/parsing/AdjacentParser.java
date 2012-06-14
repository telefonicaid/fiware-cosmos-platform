package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjacentParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public AdjacentParser(String line) {
        super(line, DELIMITER);
    }
    
    @Override
    public TwoInt parse() {
        try {
            return TwoInt.newBuilder()
                    .setNum1(this.parseInt())
                    .setNum2(this.parseInt())
                    .build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
