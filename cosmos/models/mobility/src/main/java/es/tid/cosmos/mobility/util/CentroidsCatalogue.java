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

package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.parsing.ClusterParser;

/**
 *
 * @author dmicol
 */
public abstract class CentroidsCatalogue {
    public static List<Cluster> load(Path input, MobilityConfiguration conf)
            throws IOException {
        FSDataInputStream in = null;
        InputStreamReader reader = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            reader = new InputStreamReader(in);
            final String separator = conf.getDataSeparator();
            return load(reader, separator);
        } catch (Exception ex) {
            Logger.get(CentroidsCatalogue.class).fatal(ex);
            throw new IOException(ex);
        } finally {
            if (reader != null) {
                try {
                    reader.close();
                } catch (IOException ignored) {
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ignored) {
                }
            }
        }
    }

    public static List<Cluster> load(Reader input, String separator)
            throws IOException {
        BufferedReader br = new BufferedReader(input);
        List<Cluster> centroids = new LinkedList<Cluster>();
        String line;
        while ((line = br.readLine()) != null) {
            Cluster cluster = new ClusterParser(line, separator).parse();
            centroids.add(cluster);
        }
        return centroids;
    }
}
