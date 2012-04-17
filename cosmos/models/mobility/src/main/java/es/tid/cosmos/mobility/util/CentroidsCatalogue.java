package es.tid.cosmos.mobility.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.parsing.ClusterParser;

/**
 *
 * @author dmicol
 */
public class CentroidsCatalogue {
    private List<Cluster> centroids;
    
    public CentroidsCatalogue(Path input, Configuration conf)
            throws IOException {
        this.init(input, conf);
    }
    
    private void init(Path input, Configuration conf) throws IOException {
        FSDataInputStream in = null;
        BufferedReader br = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            in = fs.open(input);
            br = new BufferedReader(new InputStreamReader(in));
            this.centroids = new LinkedList<Cluster>();
            String line;
            while ((line = br.readLine()) != null) {
                Cluster cluster = new ClusterParser(line).parse();
                this.centroids.add(cluster);
            }
        } catch (Exception ex) {
            this.centroids = null;
            Logger.get().fatal(ex);
            throw new IOException(ex);
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ex) {
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ex) {
                }
            }
        }
    }
    
    public List<Cluster> getCentroids() {
        return this.centroids;
    }
}
