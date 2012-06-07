package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

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
    public ClientProfile parse() {
        try {
            long userId = this.parseLong();
            int profile = this.parseInt();
            return ClientProfile.newBuilder()
                    .setProfileId(profile)
                    .setUserId(userId)
                    .build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
