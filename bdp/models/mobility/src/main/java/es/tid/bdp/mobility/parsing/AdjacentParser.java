package es.tid.bdp.mobility.parsing;

import es.tid.bdp.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjacentParser extends PipeDelimitedParser {
    public AdjacentParser(String line) {
        super(line);
    }
    
    @Override
    public TwoInt parse() {
        return TwoInt.newBuilder()
                .setNum1(this.parseInt())
                .setNum2(this.parseInt())
                .build();
    }
}
