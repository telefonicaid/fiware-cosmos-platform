package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;

/**
 *
 * @author dmicol
 */
public class BtsClustersParser extends Parser {
    private static final String DELIMITER = "\\|";
    
    public BtsClustersParser(String line) {
        super(line, DELIMITER);
    }

    @Override
    public Cluster parse() {
        Cluster.Builder cluster = Cluster.newBuilder();
        cluster.setLabel(parseInt());
        cluster.setLabelgroup(parseInt());
        cluster.setMean(parseDouble());
        cluster.setDistance(parseDouble());
        ClusterVector.Builder vector = ClusterVector.newBuilder();
        for (int i = 0; i < 96; i++) {
            vector.addComs(parseDouble());
        }
        cluster.setCoords(vector);
        return cluster.build();
    }
}
