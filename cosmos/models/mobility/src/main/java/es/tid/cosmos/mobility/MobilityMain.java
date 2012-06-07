package es.tid.cosmos.mobility;

import java.io.FileInputStream;
import java.io.InputStream;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.base.util.ArgumentParser;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.activitydensity.ActivityDensityRunner;
import es.tid.cosmos.mobility.activitydensity.profile.ActivityDensityProfileRunner;
import es.tid.cosmos.mobility.adjacentextraction.AdjacentExtractionRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.group.AggregatedMatrixGroupRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.simple.AggregatedMatrixSimpleRunner;
import es.tid.cosmos.mobility.itineraries.ItinerariesRunner;
import es.tid.cosmos.mobility.labelling.bts.BtsLabellingRunner;
import es.tid.cosmos.mobility.labelling.client.ClientLabellingRunner;
import es.tid.cosmos.mobility.labelling.clientbts.ClientBtsLabellingRunner;
import es.tid.cosmos.mobility.labelling.join.LabelJoiningRunner;
import es.tid.cosmos.mobility.labelling.secondhomes.DetectSecondHomesRunner;
import es.tid.cosmos.mobility.mivs.MivsRunner;
import es.tid.cosmos.mobility.outpois.OutPoisRunner;
import es.tid.cosmos.mobility.parsing.ParsingRunner;
import es.tid.cosmos.mobility.pois.PoisRunner;
import es.tid.cosmos.mobility.populationdensity.PopulationDensityRunner;
import es.tid.cosmos.mobility.populationdensity.profile.PopulationDensityProfileRunner;
import es.tid.cosmos.mobility.preparing.PreparingRunner;

/**
 *
 * @author dmicol
 */
public class MobilityMain extends Configured implements Tool {
    @Override
    public int run(String[] args) throws Exception {
        ArgumentParser arguments = new ArgumentParser();
        arguments.parse(args);
        
        InputStream configInput;
        if (arguments.has("config")) {
            configInput = new FileInputStream(arguments.getString("config"));
        } else {
            configInput = Config.class.getResource("/mobility.properties")
                    .openStream();
        }
        final Configuration conf = Config.load(configInput, this.getConf());
        
        Path outputPath = new Path(arguments.getString("output", true));
        Path cdrsPath = new Path(arguments.getString("cdrs", true));
        Path cellsPath = new Path(arguments.getString("cells", true));
        Path cellGroupsPath = new Path(arguments.getString("cellGroups", true));
        Path adjBtsPath = new Path(arguments.getString("adjBts", true));
        Path btsVectorTxtPath = new Path(arguments.getString("btsVectorTxt",
                                                             true));
        Path clientProfilePath = new Path(arguments.getString("clientProfile",
                                                              true));
        
        boolean shouldRunAll = arguments.getBoolean("runAll");
        boolean isDebug = arguments.getBoolean("debug");
        
        Path tmpParsingPath = new Path(outputPath, "parsing");
        Path cdrsMobPath = new Path(tmpParsingPath, "cdrs_mob");
        Path cellsMobPath = new Path(tmpParsingPath, "cells_mob");
        Path pairbtsAdjPath = new Path(tmpParsingPath, "pairbts_adj");
        Path btsComareaPath = new Path(tmpParsingPath, "bts_comarea");
        Path clientProfileMobPath = new Path(tmpParsingPath,
                                             "clientprofile_mob");
        boolean shouldParse = arguments.getBoolean("parse");
        if (shouldRunAll || shouldParse) {
            ParsingRunner.run(cdrsPath, cdrsMobPath, cellsPath, cellsMobPath,
                              adjBtsPath, pairbtsAdjPath, btsVectorTxtPath,
                              btsComareaPath, clientProfilePath,
                              clientProfileMobPath, conf);
        }
        
        Path tmpPreparingPath = new Path(outputPath, "preparing");
        Path cdrsInfoPath = new Path(tmpPreparingPath, "cdrs_info");
        Path cdrsNoinfoPath = new Path(tmpPreparingPath, "cdrs_noinfo");
        Path clientsBtsPath = new Path(tmpPreparingPath, "clients_bts");
        Path btsCommsPath = new Path(tmpPreparingPath, "bts_comms");
        Path cdrsNoBtsPath = new Path(tmpPreparingPath, "cdrs_no_bts");
        Path viTelmonthBtsPath = new Path(tmpPreparingPath, "vi_telmonth_bts");
        boolean shouldPrepare = arguments.getBoolean("prepare");
        if (shouldRunAll || shouldPrepare) {
            PreparingRunner.run(tmpPreparingPath, cdrsMobPath, cdrsInfoPath,
                                cdrsNoinfoPath, cellsPath, clientsBtsPath,
                                btsCommsPath, cdrsNoBtsPath, viTelmonthBtsPath,
                                conf);
        }

        Path tmpExtractMivsPath = new Path(outputPath, "mivs");
        Path viClientFuseAccPath = new Path(tmpExtractMivsPath,
                                            "vi_client_fuse_acc");
        boolean shouldExtractMivs = arguments.getBoolean("extractMIVs");
        if (shouldRunAll || shouldExtractMivs) {
            MivsRunner.run(viTelmonthBtsPath, viClientFuseAccPath,
                           tmpExtractMivsPath, isDebug, conf);
        }
        
        Path tmpExtractPoisPath = new Path(outputPath, "pois");
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

        Path tmpLabelClientPath = new Path(outputPath, "label_client");
        Path vectorClientClusterPath = new Path(tmpLabelClientPath,
                                                "vector_client_cluster");
        boolean shouldLabelClient = arguments.getBoolean("labelClient");
        if (shouldRunAll || shouldLabelClient) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_client", true));
            ClientLabellingRunner.run(cdrsMobPath, clientsInfoFilteredPath,
                                      centroidsPath, vectorClientClusterPath,
                                      tmpLabelClientPath, isDebug, conf);
        }

        Path tmpLabelBtsPath = new Path(outputPath, "label_bts");
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

        Path tmpLabelClientbtsPath = new Path(outputPath, "label_clientbts");
        Path vectorClientbtsPath = new Path(tmpLabelClientbtsPath,
                                            "vector_clientbts");
        Path pointsOfInterestTempPath = new Path(tmpLabelClientbtsPath,
                                                 "points_of_interest_temp");
        Path vectorClientbtsClusterPath = new Path(tmpLabelClientbtsPath,
                                                   "vector_clientbts_cluster");
        boolean shouldLabelClientbts = arguments.getBoolean("labelClientBTS");
        if (shouldRunAll || shouldLabelClientbts) {
            Path centroidsPath = new Path(arguments.getString(
                    "centroids_clientbts", true));
            ClientBtsLabellingRunner.run(clientsInfoPath, clientsRepbtsPath,
                                         vectorClientbtsPath, centroidsPath,
                                         pointsOfInterestTempPath,
                                         vectorClientbtsClusterPath,
                                         tmpLabelClientbtsPath, isDebug, conf);
        }

        Path tmpLabelJoining = new Path(outputPath, "label_joining");
        Path pointsOfInterestTemp4Path = new Path(tmpLabelJoining,
                                                  "points_of_interest_temp4");
        boolean shouldJoinLabels = arguments.getBoolean("joinLabels");
        if (shouldRunAll || shouldJoinLabels) {
            LabelJoiningRunner.run(pointsOfInterestTempPath,
                                   vectorClientClusterPath,
                                   vectorClientbtsClusterPath,
                                   vectorBtsClusterPath,
                                   pointsOfInterestTemp4Path,
                                   tmpLabelJoining, isDebug, conf);
        }
        
        Path tmpSecondHomesPath = new Path(outputPath, "second_homes");
        Path pointsOfInterestPath = new Path(tmpSecondHomesPath,
                                             "points_of_interest");
        boolean shouldDetectSecondHomes = arguments.getBoolean(
                "detectSecondHomes");
        if (shouldRunAll || shouldDetectSecondHomes) {
            DetectSecondHomesRunner.run(cellsMobPath, pointsOfInterestTemp4Path,
                                        viClientFuseAccPath, pairbtsAdjPath,
                                        pointsOfInterestPath,
                                        tmpSecondHomesPath, isDebug, conf);
        }
        
        Path tmpAdjacentsPath = new Path(outputPath, "adjacents");
        Path pointsOfInterestIdPath = new Path(tmpAdjacentsPath,
                                               "points_of_interest_id");
        boolean shouldExtractAdjacents = arguments.getBoolean(
                "extractAdjacents");
        if (shouldRunAll || shouldExtractAdjacents) {
            AdjacentExtractionRunner.run(pointsOfInterestPath, pairbtsAdjPath,
                                         pointsOfInterestIdPath,
                                         tmpAdjacentsPath, isDebug, conf);
        }
        
        Path tmpOutPoisPath = new Path(outputPath, "out_pois");
        boolean shouldOutPois = arguments.getBoolean("outPois");
        if (shouldRunAll || shouldOutPois) {
            OutPoisRunner.run(vectorClientbtsPath, pointsOfInterestIdPath,
                              vectorClientClusterPath, vectorBtsClusterPath,
                              tmpOutPoisPath, isDebug, conf);
        }
        
        Path tmpItinerariesPath = new Path(outputPath, "itineraries");
        Path clientItinerariesTxtPath = new Path(tmpItinerariesPath,
                                                 "client_itineraries_txt");
        boolean shouldGetItineraries = arguments.getBoolean("getItineraries");
        if (shouldRunAll || shouldGetItineraries) {
            ItinerariesRunner.run(cellsPath, cdrsInfoPath,
                                  pointsOfInterestIdPath,
                                  clientItinerariesTxtPath, tmpItinerariesPath,
                                  isDebug, conf);
        }

        Path tmpActivityDensityPath = new Path(outputPath, "activity_density");
        Path activityDensityOutPath = new Path(tmpActivityDensityPath,
                                               "activityDensityOut");
        boolean shouldGetActivityDensity = arguments.getBoolean(
                "getActivityDensity");
        if (shouldRunAll || shouldGetActivityDensity) {
            ActivityDensityRunner.run(clientsInfoPath,
                                      activityDensityOutPath,
                                      tmpActivityDensityPath, isDebug,
                                      conf);
        }
        
        Path tmpActivityDensityProfilePath = new Path(outputPath,
                "activity_density_profile");
        Path activityDensityProfileOutPath = new Path(
                tmpActivityDensityProfilePath, "activityDensityProfileOut");
        boolean shouldGetActivityDensityProfile = arguments.getBoolean(
                "getActivityDensityProfile");
        if (shouldRunAll || shouldGetActivityDensityProfile) {
            ActivityDensityProfileRunner.run(clientProfilePath,
                                             clientsInfoPath,
                                             activityDensityProfileOutPath,
                                             tmpActivityDensityProfilePath,
                                             isDebug, conf);
        }
        
        Path tmpPopulationDensityPath = new Path(outputPath,
                                                 "population_density");
        Path populationDensityOutPath = new Path(tmpPopulationDensityPath,
                                               "populationDensityOut");
        boolean shouldGetPopulationDensity = arguments.getBoolean(
                "getPopulationDensity");
        if (shouldRunAll || shouldGetPopulationDensity) {
            PopulationDensityRunner.run(cdrsInfoPath,
                                        cellsPath,
                                        clientProfileMobPath,
                                        populationDensityOutPath,
                                        tmpActivityDensityPath, isDebug,
                                        conf);
        }

        Path tmpPopulationDensityProfilePath = new Path(outputPath,
                "population_density_profile");
        Path populationDensityProfileOutPath = new Path(
                tmpPopulationDensityProfilePath, "populationDensityProfileOut");
        boolean shouldGetPopulationDensityProfile = arguments.getBoolean(
                "getPopulationDensityProfile");
        if (shouldRunAll || shouldGetPopulationDensityProfile) {
            PopulationDensityProfileRunner.run(cdrsInfoPath,
                                               cellsPath,
                                               clientProfileMobPath,
                                               populationDensityProfileOutPath,
                                               tmpPopulationDensityProfilePath,
                                               isDebug, conf);
        }
        
        Path tmpAggregatedMatrixSimplePath = new Path(outputPath,
                "aggregated_matrix_simple");
        Path matrixPairBtsTxtPath = new Path(tmpAggregatedMatrixSimplePath,
                                             "matrixPairBtsTxt");
        boolean shouldGetAggregatedMatrixSimpleProfile = arguments.getBoolean(
                "getAggregatedMatrixSimple");
        if (shouldRunAll || shouldGetAggregatedMatrixSimpleProfile) {
            AggregatedMatrixSimpleRunner.run(cdrsInfoPath, cellsPath,
                                             matrixPairBtsTxtPath,
                                             tmpAggregatedMatrixSimplePath,
                                             isDebug, conf);
        }

        Path tmpAggregatedMatrixGroupPath = new Path(outputPath,
                                                     "aggregated_matrix_group");
        Path matrixPairGroupTxtPath = new Path(tmpAggregatedMatrixGroupPath,
                                               "matrixPairGroupTxt");
        boolean shouldGetAggregatedMatrixGroupProfile = arguments.getBoolean(
                "getAggregatedMatrixGroup");
        if (shouldRunAll || shouldGetAggregatedMatrixGroupProfile) {
            AggregatedMatrixGroupRunner.run(cdrsInfoPath, cellGroupsPath,
                                            matrixPairGroupTxtPath,
                                            tmpAggregatedMatrixGroupPath,
                                            isDebug, conf);
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
            Logger.get(MobilityMain.class).fatal(ex);
            throw ex;
        }
    }
}
