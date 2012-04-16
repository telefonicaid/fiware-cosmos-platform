package es.tid.cosmos.mobility;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.mobility.parsing.ParsingRunner;
import es.tid.cosmos.mobility.preparing.PreparingRunner;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(MobilityMain.class);
    
    @Override
    public int run(String[] args) throws Exception {
        Map<String, String> arguments = new HashMap<String, String>();
        for (String arg : args) {
            String[] fields = arg.split("=");
            if (fields.length != 2) {
                throw new IllegalArgumentException("Invalid command line");
            }
            arguments.put(fields[0].replaceAll("--", ""), fields[1]);
        } 
        
        Path tmpPath = new Path("/tmp/mobility");
        Path cdrsPath = new Path(arguments.get("cdrs"));
        Path cellsPath = new Path(arguments.get("cells"));
        Path adjBtsPath = new Path(arguments.get("adjBts"));
        Path btsVectorTxtPath = new Path(arguments.get("btsVectorTxt"));
        
        Path tmpPathParsing = tmpPath.suffix("/parsing");
        Path cdrsMobPath = tmpPathParsing.suffix("/cdrs_mob");
        Path cellsMobPath = tmpPathParsing.suffix("/cells_mob");
        Path pairbtsAdjPath = tmpPathParsing.suffix("/pairbts_adj");
        Path btsComareaPath = tmpPathParsing.suffix("/bts_comarea");
        boolean shouldParse = "true".equals(arguments.get("parse"));
        if (shouldParse) {
            ParsingRunner.run(cdrsPath, cdrsMobPath, cellsPath, cellsMobPath,
                              adjBtsPath, pairbtsAdjPath, btsVectorTxtPath,
                              btsComareaPath, this.getConf());
        }
        
        Path tmpPathPreparing = tmpPath.suffix("/preparing");
        Path cdrsInfoPath = tmpPathPreparing.suffix("/cdrs_info");
        Path cdrsNoinfoPath = tmpPathPreparing.suffix("/cdrs_noinfo");
        Path clientsBtsPath = tmpPathPreparing.suffix("/clients_bts");
        Path btsCommsPath = tmpPathPreparing.suffix("/bts_comms");
        Path cdrsNoBtsPath = tmpPathPreparing.suffix("/cdrs_no_bts");
        Path viTelmonthBtsPath = tmpPathPreparing.suffix("/vi_telmonth_bts");
        boolean shouldPrepare = "true".equals(arguments.get("prepare"));
        if (shouldPrepare) {
            PreparingRunner.run(tmpPathPreparing, cdrsMobPath, cdrsInfoPath,
                                cdrsNoinfoPath, cellsMobPath, clientsBtsPath,
                                btsCommsPath, cdrsNoBtsPath, viTelmonthBtsPath,
                                this.getConf());
        }
        
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
