package es.tid.cosmos.mobility;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.mobility.btslabelling.BtsLabellingRunner;
import es.tid.cosmos.mobility.clientbtslabelling.ClientBtsLabellingRunner;
import es.tid.cosmos.mobility.clientlabelling.ClientLabellingRunner;
import es.tid.cosmos.mobility.labeljoining.LabelJoiningRunner;
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
        ArgumentParser arguments = new ArgumentParser();
        arguments.parse(args);
        final Configuration conf = this.getConf();
        
        Path tmpPath;
        if (arguments.has("tmpDir")) {
            tmpPath = new Path(arguments.getString("tmpDir"));
        } else {
            tmpPath = new Path("/tmp/mobility");
        }
        
        Path cdrsPath = new Path(arguments.getString("cdrs"));
        Path cellsPath = new Path(arguments.getString("cells"));
        Path adjBtsPath = new Path(arguments.getString("adjBts"));
        Path btsVectorTxtPath = new Path(arguments.getString("btsVectorTxt"));
        
        boolean shouldRunAll = arguments.getBoolean("run_all");
        boolean isDebug = arguments.getBoolean("debug");
        
        Path tmpParsingPath = new Path(tmpPath, "parsing");
        Path cdrsMobPath = new Path(tmpParsingPath, "cdrs_mob");
        Path cellsMobPath = new Path(tmpParsingPath, "cells_mob");
        Path pairbtsAdjPath = new Path(tmpParsingPath, "pairbts_adj");
        Path btsComareaPath = new Path(tmpParsingPath, "bts_comarea");
        boolean shouldParse = arguments.getBoolean("parse");
        if (shouldRunAll || shouldParse) {
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
        boolean shouldPrepare = arguments.getBoolean("prepare");
        if (shouldRunAll || shouldPrepare) {
            PreparingRunner.run(tmpPreparingPath, cdrsMobPath, cdrsInfoPath,
                                cdrsNoinfoPath, cellsMobPath, clientsBtsPath,
                                btsCommsPath, cdrsNoBtsPath, viTelmonthBtsPath,
                                conf);
        }
        
        Path tmpExtractPoisPath = new Path(tmpPath, "extract_pois");
        Path clientsInfoPath = new Path(tmpExtractPoisPath, "clients_info");
        Path clientsInfoFilteredPath = new Path(tmpExtractPoisPath,
                                                "clients_info_filtered");
        Path clientsRepbtsPath = new Path(tmpExtractPoisPath, "clients_repbts");
        boolean shouldExtractPois = arguments.getBoolean("extractPOIs");
        if (shouldRunAll || shouldExtractPois) {
            PoisRunner.run(tmpExtractPoisPath, clientsBtsPath, clientsInfoPath,
                           cdrsNoinfoPath, cdrsNoBtsPath,
                           clientsInfoFilteredPath, clientsRepbtsPath, isDebug,
                           conf);
        }

        Path tmpLabelClientPath = new Path(tmpPath, "label_client");
        Path vectorClientClusterPath = new Path(tmpLabelClientPath,
                                                "vector_client_clusterPath");
        boolean shouldLabelClient = arguments.getBoolean("labelClient");
        if (shouldRunAll || shouldLabelClient) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_client", true));
            ClientLabellingRunner.run(cdrsMobPath, clientsInfoFilteredPath,
                                      centroidsPath, vectorClientClusterPath,
                                      tmpLabelClientPath, isDebug, conf);
        }

        Path tmpLabelBtsPath = new Path(tmpPath, "label_bts");
        Path vectorBtsClusterPath = new Path(tmpLabelBtsPath,
                                             "vector_bts_cluster");
        boolean shouldLabelBts = arguments.getBoolean("labelBTS");
        if (shouldRunAll || shouldLabelBts) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_bts", true));
            BtsLabellingRunner.run(btsCommsPath, btsComareaPath,
                                   centroidsPath, vectorBtsClusterPath,
                                   tmpLabelBtsPath, isDebug, conf);
        }

        Path tmpLabelClientbtsPath = new Path(tmpPath, "label_clientbts");
        Path pointsOfInterestTempPath = new Path(tmpLabelClientbtsPath,
                                                 "vector_clientbts_cluster");
        Path vectorClientbtsClusterPath = new Path(tmpLabelClientbtsPath,
                                                   "vector_clientbts_cluster");
        boolean shouldLabelClientbts = arguments.getBoolean("labelClientBTS");
        if (shouldRunAll || shouldLabelClientbts) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_clientbts", true));
            ClientBtsLabellingRunner.run(clientsInfoPath, clientsRepbtsPath,
                    centroidsPath, pointsOfInterestTempPath,
                    vectorClientbtsClusterPath, tmpLabelBtsPath, isDebug, conf);
        }

        Path tmpLabelJoining = new Path(tmpPath, "label_joining");
        boolean shouldJoinLabels = arguments.getBoolean("joinLabels");
        if (shouldRunAll || shouldJoinLabels) {
            LabelJoiningRunner.run(pointsOfInterestTempPath,
                    vectorClientClusterPath, vectorClientbtsClusterPath,
                    vectorBtsClusterPath, tmpLabelJoining, conf);
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
            Logger.get().fatal(ex);
            throw ex;
        }
    }
}
