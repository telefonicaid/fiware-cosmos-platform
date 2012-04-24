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
        cluster.setLabel(this.parseInt());
        cluster.setLabelgroup(this.parseInt());
        cluster.setMean(this.parseDouble());
        cluster.setDistance(this.parseDouble());
        cluster.setCoords(this.parseClusterVector());
        return cluster.build();
    }
}
