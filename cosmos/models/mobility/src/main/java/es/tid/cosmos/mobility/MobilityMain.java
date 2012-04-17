package es.tid.cosmos.mobility;

import java.util.HashMap;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.mobility.parsing.ParsingRunner;
import es.tid.cosmos.mobility.pois.PoisRunner;
import es.tid.cosmos.mobility.preparing.PreparingRunner;
import es.tid.cosmos.mobility.util.Logger;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
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
        
        Path tmpParsingPath = new Path(tmpPath, "parsing");
        Path cdrsMobPath = new Path(tmpParsingPath, "cdrs_mob");
        Path cellsMobPath = new Path(tmpParsingPath, "cells_mob");
        Path pairbtsAdjPath = new Path(tmpParsingPath, "pairbts_adj");
        Path btsComareaPath = new Path(tmpParsingPath, "bts_comarea");
        boolean shouldParse = "true".equals(arguments.get("parse"));
        if (shouldParse) {
            ParsingRunner.run(cdrsPath, cdrsMobPath, cellsPath, cellsMobPath,
                              adjBtsPath, pairbtsAdjPath, btsVectorTxtPath,
                              btsComareaPath, conf);
        }
        
        Path tmpPreparingPath = new Path(tmpPath, "preparing");
        Path cdrsInfoPath = new Path(tmpPreparingPath, "cdrs_info");
        Path cdrsNoinfoPath = new Path(tmpPreparingPath, "cdrs_noinfo");
        Path clientsBtsPath = new Path(tmpPreparingPath, "clients_bts");
        Path btsCommsPath = new Path(tmpPreparingPath, "bts_comms");
        Path cdrsNoBtsPath = new Path(tmpPreparingPath, "cdrs_no_bts");
        Path viTelmonthBtsPath = new Path(tmpPreparingPath, "vi_telmonth_bts");
        boolean shouldPrepare = "true".equals(arguments.get("prepare"));
        if (shouldPrepare) {
            PreparingRunner.run(tmpPreparingPath, cdrsMobPath, cdrsInfoPath,
                                cdrsNoinfoPath, cellsMobPath, clientsBtsPath,
                                btsCommsPath, cdrsNoBtsPath, viTelmonthBtsPath,
                                conf);
        }
        
        Path tmpExtractPoisPath = new Path(tmpPath, "extract_pois");
        Path clientsRepbtsPath = new Path(tmpExtractPoisPath, "clients_repbts");
        boolean shouldExtractPois = "true".equals(arguments.get("extractPOIs"));
        if (shouldExtractPois) {
            PoisRunner.run(tmpExtractPoisPath, clientsBtsPath, cdrsNoinfoPath,
                           cdrsNoBtsPath,  clientsRepbtsPath, conf);
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
            Logger.get().fatal(ex);
            throw ex;
        }
    }
}
