package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;

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
            cluster.setLabel(parseInt());
            cluster.setLabelgroup(parseInt());
            cluster.setMean(parseDouble());
            cluster.setDistance(parseDouble());
            ClusterVector.Builder clusterVector = ClusterVector.newBuilder();
            for (int i = 0; i < 96; i++) {
                clusterVector.addComs(parseDouble());
            }
            cluster.setCoords(clusterVector.build());
            return cluster.build();
        } catch (Exception ex) {
            throw new IllegalArgumentException("Failed to parse: " + this.line);
        }
    }
}
