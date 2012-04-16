package es.tid.cosmos.mobility;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.mobility.parsing.ParsingRunner;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(MobilityMain.class);
    
    @Override
    public int run(String[] args) throws Exception {
        if (args.length != 4) {
            throw new IllegalArgumentException("Mandatory parameters: "
                                               + "cdrs_path cells_path");
        }

        Path cdrsPath = new Path(args[0]);
        Path cellsPath = new Path(args[1]);
        Path adjBtsPath = new Path(args[2]);
        Path btsComarea = new Path(args[3]);
        
        Path tmpPath = new Path("/tmp");
        Path cdrsMobPath = tmpPath.suffix("/cdrs_mob");
        Path cellsMobPath = tmpPath.suffix("/cells_mob");
        Path pairbtsAdjPath = tmpPath.suffix("/pairbts_adj");
        Path btsComareaPath = tmpPath.suffix("/bts_comarea");
        
        ParsingRunner.run(cdrsPath, cdrsMobPath, cellsPath, cellsMobPath,
                          adjBtsPath, pairbtsAdjPath, btsComarea,
                          btsComareaPath, this.getConf());
        
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        try {
            int res = ToolRunner.run(new Configuration(),
                                     new MobilityMain(), args);
            if (res != 0) {
                throw new Exception("Unknown error");
            }
        } catch (Exception ex) {
            LOG.fatal(ex);
            throw ex;
        }
    }
}
