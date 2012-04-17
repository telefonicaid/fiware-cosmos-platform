package es.tid.cosmos.mobility;

import es.tid.cosmos.mobility.util.Logger;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public abstract class Runner {
    private Configuration conf;
    
    public Runner(Configuration conf) {
        this.conf = conf;
    }
    
    abstract boolean run();
    
    protected Path createDir(String parent, String child) {
        return new Path(parent, child);
    }
    
    protected Path createFile(String parent, String child, boolean isTemp)
            throws IOException {
        FileSystem fs = FileSystem.get(this.conf);
        Path path = this.createDir(parent, child);
        if (fs.exists(path)) {
            throw new IOException("File " + path.toString() + " already exists");
        }
        if (!fs.createNewFile(path)) {
            throw new IOException("Failed to create " + path.toString());
        }
        if (isTemp) {
            if (!fs.deleteOnExit(path)) {
                Logger.get().warn("Could not set " + path.toString()
                                  + " for automatic deletion");
            }
        }
        return path;
    }
}
