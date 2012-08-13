/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.mobility.parsing;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 *
 * @author dmicol
 */
public class ClusterParser extends Parser {
    public ClusterParser(String line, String separator) {
        super(line, separator);
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
