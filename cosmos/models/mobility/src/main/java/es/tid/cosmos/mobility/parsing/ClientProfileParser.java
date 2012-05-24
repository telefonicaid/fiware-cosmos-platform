package es.tid.cosmos.mobility.parsing;

import com.twitter.elephantbird.util.Pair;

/**
 *
 * @author dmicol
 */
public class ClientProfileParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public ClientProfileParser(String line) {
        super(line, DELIMITER);
    }

    @Override
    public Pair<Long, Integer> parse() {
        try {
            long nodeId = this.parseLong();
            int profile = this.parseInt();
            return new Pair<Long, Integer>(nodeId, profile);
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
