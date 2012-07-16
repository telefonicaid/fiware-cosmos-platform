package es.tid.cosmos.mobility;

import java.io.FileInputStream;
import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

import es.tid.cosmos.base.mapreduce.CosmosWorkflow;
import es.tid.cosmos.base.mapreduce.WorkflowList;
import es.tid.cosmos.base.util.ArgumentParser;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.activitydensity.ActivityDensityRunner;
import es.tid.cosmos.mobility.activitydensity.profile.ActivityDensityProfileRunner;
import es.tid.cosmos.mobility.adjacentextraction.AdjacentExtractionRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.group.AggregatedMatrixGroupRunner;
import es.tid.cosmos.mobility.aggregatedmatrix.simple.AggregatedMatrixSimpleRunner;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
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

    public static final String CONFIG_FLAG = "config";

    @Override
    public int run(String[] args) throws IOException, InterruptedException,
                                         ClassNotFoundException {
        WorkflowList wfList = new WorkflowList();
        ArgumentParser arguments = new ArgumentParser();
        arguments.parse(args);

        // Override the actual configuration with a mobility-based one, in order
        // to have the corresponding execution parameters
        final MobilityConfiguration conf = new MobilityConfiguration(
                this.getConf());
        this.setConf(conf);
        if (arguments.has(CONFIG_FLAG)) {
            conf.load(new FileInputStream(arguments.getString(CONFIG_FLAG)));
        }

        if (!conf.getSysExecMode().equalsIgnoreCase("complete")) {
            throw new UnsupportedOperationException(
                    "Only complete execution mode is supported");
        }
        if (conf.getSysExecIncremental()) {
            throw new UnsupportedOperationException(
                    "Incremental mode is not supported yet");
        }

        Path inputPath = new Path(conf.getSysInputFolder());
        Path outputPath = new Path(conf.getSysOutputCompleteFolder());
        Path cdrsPath = new Path(inputPath, "cdrs");
        Path cellsPath = getOnlyFileInDirectory(
                new Path(inputPath, "cells"), conf);
        Path cellGroupsPath = getOnlyFileInDirectory(
                new Path(inputPath, "cellGroups"), conf);
        Path adjBtsPath = new Path(inputPath, "adjBts");
        Path btsVectorTxtPath = new Path(inputPath, "btsVector");
        Path clientProfilePath = new Path(inputPath, "clientProfile");

        /*
         * Extract the list of phases to execute
         */
        boolean shouldParse = arguments.getBoolean("parse");
        boolean shouldPrepare = arguments.getBoolean("prepare");
        boolean shouldExtractMivs = arguments.getBoolean("extractMIVs");
        boolean shouldExtractPois = arguments.getBoolean("extractPOIs");
        boolean shouldLabelClient = arguments.getBoolean("labelClient");
        boolean shouldLabelBts = arguments.getBoolean("labelBTS");
        boolean shouldLabelClientbts = arguments.getBoolean("labelClientBTS");
        boolean shouldJoinLabels = arguments.getBoolean("joinLabels");
        boolean shouldDetectSecondHomes = arguments.getBoolean(
                "detectSecondHomes");
        boolean shouldGetActivityDensity = arguments.getBoolean(
                "getActivityDensity");
        boolean shouldGetActivityDensityProfile = arguments.getBoolean(
                "getActivityDensityProfile");
        boolean shouldGetPopulationDensity = arguments.getBoolean(
                "getPopulationDensity");
        boolean shouldGetPopulationDensityProfile = arguments.getBoolean(
                "getPopulationDensityProfile");
        boolean shouldGetAggregatedMatrixSimpleProfile = arguments.getBoolean(
                "getAggregatedMatrixSimple");
        boolean shouldGetAggregatedMatrixGroupProfile = arguments.getBoolean(
                "getAggregatedMatrixGroup");
        boolean shouldExtractAdjacents = arguments.getBoolean(
                "extractAdjacents");
        boolean shouldGetItineraries = arguments.getBoolean("getItineraries");
        boolean shouldOutPois = arguments.getBoolean("outPois");
        boolean shouldRunAll = !(shouldParse || shouldPrepare || shouldExtractMivs
                || shouldExtractPois || shouldLabelClient || shouldLabelBts
                || shouldLabelClientbts || shouldJoinLabels
                || shouldDetectSecondHomes || shouldGetActivityDensity
                || shouldGetActivityDensityProfile || shouldGetPopulationDensity
                || shouldGetPopulationDensity || shouldGetPopulationDensityProfile
                || shouldGetAggregatedMatrixSimpleProfile
                || shouldGetAggregatedMatrixGroupProfile
                || shouldExtractAdjacents || shouldGetItineraries || shouldOutPois);
        boolean isDebug = arguments.getBoolean("debug");

        Path tmpParsingPath = new Path(outputPath, "parsing");
        Path cdrsMobPath = new Path(tmpParsingPath, "cdrs_mob");
        Path cellsMobPath = new Path(tmpParsingPath, "cells_mob");
        Path pairbtsAdjPath = new Path(tmpParsingPath, "pairbts_adj");
        Path btsComareaPath = new Path(tmpParsingPath, "bts_comarea");
        Path clientProfileMobPath = new Path(tmpParsingPath,
                                             "clientprofile_mob");
        CosmosWorkflow parsingWorkflow = null;
        if (shouldRunAll || shouldParse) {
            parsingWorkflow = ParsingRunner.run(cdrsPath, cdrsMobPath,
                    cellsPath, cellsMobPath, adjBtsPath, pairbtsAdjPath,
                    btsVectorTxtPath, btsComareaPath, clientProfilePath,
                    clientProfileMobPath, isDebug, conf);
            wfList.add(parsingWorkflow);
        }

        Path tmpPreparingPath = new Path(outputPath, "preparing");
        Path cdrsInfoPath = new Path(tmpPreparingPath, "cdrs_info");
        Path cdrsNoinfoPath = new Path(tmpPreparingPath, "cdrs_noinfo");
        Path clientsBtsPath = new Path(tmpPreparingPath, "clients_bts");
        Path btsCommsPath = new Path(tmpPreparingPath, "bts_comms");
        Path cdrsNoBtsPath = new Path(tmpPreparingPath, "cdrs_no_bts");
        Path viTelmonthBtsPath = new Path(tmpPreparingPath, "vi_telmonth_bts");
        CosmosWorkflow preparingWorkflow = null;
        if (shouldRunAll || shouldPrepare) {
            preparingWorkflow = PreparingRunner.run(tmpPreparingPath,
                    cdrsMobPath, cdrsInfoPath, cdrsNoinfoPath, cellsPath,
                    clientsBtsPath, btsCommsPath, cdrsNoBtsPath,
                    viTelmonthBtsPath, isDebug, conf);
            preparingWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(preparingWorkflow);
        }

        Path tmpExtractMivsPath = new Path(outputPath, "mivs");
        Path viClientFuseAccPath = new Path(tmpExtractMivsPath,
                                            "vi_client_fuse_acc");
        CosmosWorkflow mivsWorkflow = null;
        if (shouldRunAll || shouldExtractMivs) {
            mivsWorkflow = MivsRunner.run(viTelmonthBtsPath, viClientFuseAccPath,
                    tmpExtractMivsPath, isDebug, conf);
            mivsWorkflow.addDependentWorkflow(preparingWorkflow);
            wfList.add(mivsWorkflow);
        }

        Path tmpExtractPoisPath = new Path(outputPath, "pois");
        Path clientsInfoPath = new Path(tmpExtractPoisPath, "clients_info");
        Path clientsInfoFilteredPath = new Path(tmpExtractPoisPath,
                                                "clients_info_filtered");
        Path clientsRepbtsPath = new Path(tmpExtractPoisPath, "clients_repbts");
        CosmosWorkflow poisWorkflow = null;
        if (shouldRunAll || shouldExtractPois) {
            poisWorkflow = PoisRunner.run(tmpExtractPoisPath, clientsBtsPath,
                    clientsInfoPath, cdrsNoinfoPath, cdrsNoBtsPath,
                    clientsInfoFilteredPath, clientsRepbtsPath, isDebug, conf);
            poisWorkflow.addDependentWorkflow(preparingWorkflow);
            wfList.add(poisWorkflow);
        }

        Path tmpLabelClientPath = new Path(outputPath, "label_client");
        Path vectorClientClusterPath = new Path(tmpLabelClientPath,
                                                "vector_client_cluster");
        CosmosWorkflow clientLabellingWorkflow = null;
        if (shouldRunAll || shouldLabelClient) {
            Path centroidsPath = getOnlyFileInDirectory(
                    new Path(inputPath, "centroidsClient"), conf);
            clientLabellingWorkflow = ClientLabellingRunner.run(cdrsMobPath,
                    clientsInfoFilteredPath, centroidsPath,
                    vectorClientClusterPath, tmpLabelClientPath, isDebug, conf);
            clientLabellingWorkflow.addDependentWorkflow(parsingWorkflow);
            clientLabellingWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(clientLabellingWorkflow);
        }

        Path tmpLabelBtsPath = new Path(outputPath, "label_bts");
        Path vectorBtsClusterPath = new Path(tmpLabelBtsPath,
                                             "vector_bts_cluster");
        CosmosWorkflow btsLabellingWorkflow = null;
        if (shouldRunAll || shouldLabelBts) {
            Path centroidsPath = getOnlyFileInDirectory(
                    new Path(inputPath, "centroidsBts"), conf);
            btsLabellingWorkflow = BtsLabellingRunner.run(btsCommsPath,
                    btsComareaPath, centroidsPath, vectorBtsClusterPath,
                    tmpLabelBtsPath, isDebug, conf);
            btsLabellingWorkflow.addDependentWorkflow(parsingWorkflow);
            btsLabellingWorkflow.addDependentWorkflow(preparingWorkflow);
            wfList.add(btsLabellingWorkflow);
        }

        Path tmpLabelClientbtsPath = new Path(outputPath, "label_clientbts");
        Path vectorClientbtsPath = new Path(tmpLabelClientbtsPath,
                                            "vector_clientbts");
        Path pointsOfInterestTempPath = new Path(tmpLabelClientbtsPath,
                                                 "points_of_interest_temp");
        Path vectorClientbtsClusterPath = new Path(tmpLabelClientbtsPath,
                                                   "vector_clientbts_cluster");
        CosmosWorkflow clientBtsLabellingWorkflow = null;
        if (shouldRunAll || shouldLabelClientbts) {
            Path medoidsPath = getOnlyFileInDirectory(
                    new Path(inputPath, "medoidsClientbts"), conf);
            clientBtsLabellingWorkflow = ClientBtsLabellingRunner.run(
                    clientsInfoPath, clientsRepbtsPath, vectorClientbtsPath,
                    medoidsPath, pointsOfInterestTempPath,
                    vectorClientbtsClusterPath, tmpLabelClientbtsPath, isDebug,
                    conf);
            clientBtsLabellingWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(clientBtsLabellingWorkflow);
        }

        Path tmpLabelJoining = new Path(outputPath, "label_joining");
        Path pointsOfInterestTemp4Path = new Path(tmpLabelJoining,
                                                  "points_of_interest_temp4");
        CosmosWorkflow labelJoiningWorkflow = null;
        if (shouldRunAll || shouldJoinLabels) {
            labelJoiningWorkflow = LabelJoiningRunner.run(
                    pointsOfInterestTempPath, vectorClientClusterPath,
                    vectorClientbtsClusterPath, vectorBtsClusterPath,
                    pointsOfInterestTemp4Path, tmpLabelJoining, isDebug, conf);
            labelJoiningWorkflow.addDependentWorkflow(clientBtsLabellingWorkflow);
            labelJoiningWorkflow.addDependentWorkflow(clientLabellingWorkflow);
            labelJoiningWorkflow.addDependentWorkflow(btsLabellingWorkflow);
            wfList.add(labelJoiningWorkflow);
        }

        Path tmpSecondHomesPath = new Path(outputPath, "second_homes");
        Path pointsOfInterestPath = new Path(tmpSecondHomesPath,
                                             "points_of_interest");
        if (shouldRunAll || shouldDetectSecondHomes) {
            CosmosWorkflow detectSecondHomesWorkflow = DetectSecondHomesRunner.run(
                    cellsMobPath, pointsOfInterestTemp4Path, viClientFuseAccPath,
                    pairbtsAdjPath, pointsOfInterestPath, tmpSecondHomesPath,
                    isDebug, conf);
            detectSecondHomesWorkflow.addDependentWorkflow(parsingWorkflow);
            detectSecondHomesWorkflow.addDependentWorkflow(labelJoiningWorkflow);
            detectSecondHomesWorkflow.addDependentWorkflow(mivsWorkflow);
            wfList.add(detectSecondHomesWorkflow);
        }

        Path tmpActivityDensityPath = new Path(outputPath, "activity_density");
        Path activityDensityOutPath = new Path(tmpActivityDensityPath,
                                               "activityDensityOut");
        if (shouldRunAll || shouldGetActivityDensity) {
            CosmosWorkflow activityDensityWorkflow = ActivityDensityRunner.run(
                    clientsInfoPath, activityDensityOutPath,
                    tmpActivityDensityPath, isDebug, conf);
            activityDensityWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(activityDensityWorkflow);
        }

        Path tmpActivityDensityProfilePath = new Path(outputPath,
                "activity_density_profile");
        Path activityDensityProfileOutPath = new Path(
                tmpActivityDensityProfilePath, "activityDensityProfileOut");
        if (shouldRunAll || shouldGetActivityDensityProfile) {
            CosmosWorkflow activityDensityProfileWorkflow =
                    ActivityDensityProfileRunner.run(clientProfileMobPath,
                            clientsInfoPath, activityDensityProfileOutPath,
                            tmpActivityDensityProfilePath, isDebug, conf);
            activityDensityProfileWorkflow.addDependentWorkflow(parsingWorkflow);
            activityDensityProfileWorkflow.addDependentWorkflow(poisWorkflow);
            wfList.add(activityDensityProfileWorkflow);
        }

        Path tmpPopulationDensityPath = new Path(outputPath,
                                                 "population_density");
        Path populationDensityOutPath = new Path(tmpPopulationDensityPath,
                                               "populationDensityOut");
        if (shouldRunAll || shouldGetPopulationDensity) {
            CosmosWorkflow populationDensityWorkflow = PopulationDensityRunner.run(
                    cdrsInfoPath, cellsPath, populationDensityOutPath,
                    tmpPopulationDensityPath, isDebug, conf);
            populationDensityWorkflow.addDependentWorkflow(preparingWorkflow);
            populationDensityWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(populationDensityWorkflow);
        }

        Path tmpPopulationDensityProfilePath = new Path(outputPath,
                "population_density_profile");
        Path populationDensityProfileOutPath = new Path(
                tmpPopulationDensityProfilePath, "populationDensityProfileOut");
        if (shouldRunAll || shouldGetPopulationDensityProfile) {
            CosmosWorkflow populationDensityProfileWorkflow =
                    PopulationDensityProfileRunner.run(cdrsInfoPath, cellsPath,
                            clientProfileMobPath, populationDensityProfileOutPath,
                            tmpPopulationDensityProfilePath, isDebug, conf);
            populationDensityProfileWorkflow.addDependentWorkflow(preparingWorkflow);
            populationDensityProfileWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(populationDensityProfileWorkflow);
        }

        Path tmpAggregatedMatrixSimplePath = new Path(outputPath,
                "aggregated_matrix_simple");
        Path matrixPairBtsTxtPath = new Path(tmpAggregatedMatrixSimplePath,
                                             "matrixPairBtsTxt");
        if (shouldRunAll || shouldGetAggregatedMatrixSimpleProfile) {
            CosmosWorkflow aggregatedMatrixSimpleWorkflow =
                    AggregatedMatrixSimpleRunner.run(cdrsInfoPath, cellsPath,
                            matrixPairBtsTxtPath, tmpAggregatedMatrixSimplePath,
                            isDebug, conf);
            aggregatedMatrixSimpleWorkflow.addDependentWorkflow(preparingWorkflow);
            aggregatedMatrixSimpleWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(aggregatedMatrixSimpleWorkflow);
        }

        Path tmpAggregatedMatrixGroupPath = new Path(outputPath,
                                                     "aggregated_matrix_group");
        Path matrixPairGroupTxtPath = new Path(tmpAggregatedMatrixGroupPath,
                                               "matrixPairGroupTxt");
        if (shouldRunAll || shouldGetAggregatedMatrixGroupProfile) {
            CosmosWorkflow aggregatedMatrixGroupWorkflow =
                    AggregatedMatrixGroupRunner.run(cdrsInfoPath, cellGroupsPath,
                            matrixPairGroupTxtPath, tmpAggregatedMatrixGroupPath,
                            isDebug, conf);
            aggregatedMatrixGroupWorkflow.addDependentWorkflow(preparingWorkflow);
            aggregatedMatrixGroupWorkflow.addDependentWorkflow(parsingWorkflow);
            wfList.add(aggregatedMatrixGroupWorkflow);
        }

        wfList.waitForCompletion(true);

        // The following phases aren't modelled through CosmosWorkflows
        // because AdjancentExtractionRunner contains loops, which is currently
        // not supported by CosmosWorkflows
        Path tmpAdjacentsPath = new Path(outputPath, "adjacents");
        Path pointsOfInterestIdPath = new Path(tmpAdjacentsPath,
                                               "points_of_interest_id");
        if (shouldRunAll || shouldExtractAdjacents) {
            AdjacentExtractionRunner.run(pointsOfInterestPath, pairbtsAdjPath,
                    pointsOfInterestIdPath, tmpAdjacentsPath, isDebug, conf);
        }

        Path tmpItinerariesPath = new Path(outputPath, "itineraries");
        Path clientItinerariesTxtPath = new Path(tmpItinerariesPath,
                                                 "client_itineraries_txt");
        if (shouldRunAll || shouldGetItineraries) {
            ItinerariesRunner.run(cellsPath, cdrsInfoPath, pointsOfInterestIdPath,
                    clientItinerariesTxtPath, tmpItinerariesPath, isDebug, conf);
        }

        Path tmpOutPoisPath = new Path(outputPath, "out_pois");
        if (shouldRunAll || shouldOutPois) {
            OutPoisRunner.run(vectorClientbtsPath, pointsOfInterestIdPath,
                    vectorClientClusterPath, vectorBtsClusterPath,
                    tmpOutPoisPath, isDebug, conf);
        }

        return 0;
    }

    private static Path getOnlyFileInDirectory(Path directoryPath,
            Configuration conf) throws IOException {
        FileSystem fs = FileSystem.get(conf);
        if (!fs.exists(directoryPath)) {
            throw new IllegalArgumentException(
                    "Path " + directoryPath + " does not exist");
        }
        FileStatus[] files = fs.listStatus(directoryPath);
        if (files.length != 1) {
            throw new IllegalArgumentException(
                    "Only one file should be in " + directoryPath);
        }
        return files[0].getPath();
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
