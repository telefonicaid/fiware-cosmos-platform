package es.tid.cosmos.mobility;

import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.log4j.Logger;

import es.tid.cosmos.mobility.parsing.ParsingRunner;
import es.tid.cosmos.mobility.pois.PoisRunner;
import es.tid.cosmos.mobility.preparing.PreparingRunner;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    private static final Logger LOG = Logger.getLogger(MobilityMain.class);
    
    @Override
    public int run(String[] args) throws Exception {
        Map<String, String> arguments = parseArguments(args);
        
        Path tmpPath = new Path("/tmp/mobility");
        Path cdrsPath = new Path(arguments.get("cdrs"));
        Path cellsPath = new Path(arguments.get("cells"));
        Path adjBtsPath = new Path(arguments.get("adjBts"));
        Path btsVectorTxtPath = new Path(arguments.get("btsVectorTxt"));
        
        Configuration conf = this.getConf();
        conf.set("CELL_CATALOGUE_PATH", cellsPath.toString());
        
        Path tmpParsingPath = tmpPath.suffix("/parsing");
        Path cdrsMobPath = tmpParsingPath.suffix("/cdrs_mob");
        Path cellsMobPath = tmpParsingPath.suffix("/cells_mob");
        Path pairbtsAdjPath = tmpParsingPath.suffix("/pairbts_adj");
        Path btsComareaPath = tmpParsingPath.suffix("/bts_comarea");
        boolean shouldParse = "true".equals(arguments.get("parse"));
        if (shouldParse) {
            ParsingRunner.run(cdrsPath, cdrsMobPath, cellsPath, cellsMobPath,
                              adjBtsPath, pairbtsAdjPath, btsVectorTxtPath,
                              btsComareaPath, this.getConf());
        }
        
        Path tmpPreparingPath = tmpPath.suffix("/preparing");
        Path cdrsInfoPath = tmpPreparingPath.suffix("/cdrs_info");
        Path cdrsNoinfoPath = tmpPreparingPath.suffix("/cdrs_noinfo");
        Path clientsBtsPath = tmpPreparingPath.suffix("/clients_bts");
        Path btsCommsPath = tmpPreparingPath.suffix("/bts_comms");
        Path cdrsNoBtsPath = tmpPreparingPath.suffix("/cdrs_no_bts");
        Path viTelmonthBtsPath = tmpPreparingPath.suffix("/vi_telmonth_bts");
        boolean shouldPrepare = "true".equals(arguments.get("prepare"));
        if (shouldPrepare) {
            PreparingRunner.run(tmpPreparingPath, cdrsMobPath, cdrsInfoPath,
                                cdrsNoinfoPath, cellsMobPath, clientsBtsPath,
                                btsCommsPath, cdrsNoBtsPath, viTelmonthBtsPath,
                                this.getConf());
        }
        
        Path tmpExtractPoisPath = tmpPath.suffix("/extract_pois");
        Path clientsRepbtsPath = tmpPreparingPath.suffix("/clients_repbts");
        boolean shouldExtractPois = "true".equals(arguments.get("extractPOIs"));
        if (shouldExtractPois) {
            PoisRunner.run(tmpExtractPoisPath, clientsBtsPath,
                           clientsRepbtsPath, this.getConf());
        }
        
        return 0;
    }
    
    private static Map<String, String> parseArguments(String[] args) {
        Map<String, String> arguments = new HashMap<String, String>();
        for (String arg : args) {
            String[] fields = arg.split("=");
            if (fields.length != 2) {
                throw new IllegalArgumentException("Invalid command line");
            }
            arguments.put(fields[0].replaceAll("--", ""), fields[1]);
        }
        return arguments;
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
