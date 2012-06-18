package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 *
 * @author dmicol
 */
public class ClusterParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public ClusterParser(String line) {
        super(line, DELIMITER);
    }

    @Override
    public Cluster parse() {
        try {
            Cluster.Builder cluster = Cluster.newBuilder();
            cluster.setLabel(this.parseInt());
            cluster.setLabelgroup(this.parseInt());
            cluster.setMean(this.parseDouble());
            cluster.setDistance(this.parseDouble());
            cluster.setCoords(this.parseClusterVector());
            return cluster.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line,
                                               ex);
        }
    }
}
