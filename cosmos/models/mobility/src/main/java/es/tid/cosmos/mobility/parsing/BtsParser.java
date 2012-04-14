package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.MobProtocol.Bts;

/**
 *
 * @author dmicol
 */
public class BtsParser extends PipeDelimitedParser {
    public BtsParser(String line) {
        super(line);
    }
    
    @Override
    public Bts parse() {
        return Bts.newBuilder()
                .setPlaceId(this.parseCellId())
                .setComms(this.parseLong())
                .setArea(this.parseLong())
                .build();
    }
}
