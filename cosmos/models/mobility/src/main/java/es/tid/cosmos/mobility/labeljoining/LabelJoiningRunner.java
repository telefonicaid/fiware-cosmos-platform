package es.tid.cosmos.mobility.labeljoining;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;

/**
 *
 * @author dmicol
 */
public final class LabelJoiningRunner {
    private LabelJoiningRunner() {
    }
    
    public static void run(Path pointsOfInterestTempPath,
                           Path vectorClientClusterPath,
                           Path vectorClientbtsClusterPath,
                           Path vectorBtsClusterPath,
                           Path tmpDirPath, Configuration conf)
            throws Exception {
        Path pointsOfInterestTemp2Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp2");
        {
            ClusterAggNodeClusterByNodbtsJob job =
                    new ClusterAggNodeClusterByNodbtsJob(conf);
            job.configure(new Path[] { pointsOfInterestTempPath,
                                       vectorClientClusterPath },
                          pointsOfInterestTemp2Path);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path potpoiPath = new Path(tmpDirPath, "potpoi");
        { 
            ClusterAggNodeClusterByNodlblJob job =
                    new ClusterAggNodeClusterByNodlblJob(conf);
            job.configure(new Path[] { pointsOfInterestTempPath,
                                       vectorClientClusterPath },
                          potpoiPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path clientbtsNodpoilblPath = new Path(tmpDirPath,
                                               "clientbts_nodpoilbl");
        { 
            ClusterSpreadNodelblPoilblJob job =
                    new ClusterSpreadNodelblPoilblJob(conf);
            job.configure(pointsOfInterestTemp2Path, clientbtsNodpoilblPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path clientbtsNodpoiCountPath = new Path(tmpDirPath,
                                                 "clientbts_nodpoi_count");
        {
            ClusterCountMajPoiByNodeJob job = new ClusterCountMajPoiByNodeJob(
                    conf);
            job.configure(clientbtsNodpoilblPath, clientbtsNodpoiCountPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
        
        Path clientbtsNodPoimajPath = new Path(tmpDirPath,
                                               "clientbts_nod_poimaj");
        {
            ClusterGetMajPoiByNodeJob job = new ClusterGetMajPoiByNodeJob(conf);
            job.configure(new Path[] { potpoiPath, clientbtsNodpoiCountPath },
                          clientbtsNodPoimajPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path poisLabeledPath = new Path(tmpDirPath, "pois_labeled");
        {
            ClusterJoinPotPoiLabelJob job = new ClusterJoinPotPoiLabelJob(conf);
            job.configure(new Path[] { potpoiPath, clientbtsNodPoimajPath },
                          poisLabeledPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path pointsOffInterestTemp3Path = new Path(tmpDirPath,
                                                   "points_of_interest_temp3");
        {
            ClusterAggPotPoiPoisToPoiJob job =
                    new ClusterAggPotPoiPoisToPoiJob(conf);
            job.configure(new Path[] { pointsOfInterestTemp2Path,
                                       vectorClientbtsClusterPath,
                                       poisLabeledPath },
                          pointsOffInterestTemp3Path);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path vectorClientbtsClusterAddPath = new Path(tmpDirPath,
                "vector_clientbts_cluster_add");
        {
            ClusterAggPotPoiPoisToClusterJob job =
                    new ClusterAggPotPoiPoisToClusterJob(conf);
            job.configure(new Path[] { pointsOfInterestTemp2Path,
                                       vectorClientbtsClusterPath,
                                       poisLabeledPath },
                          vectorClientbtsClusterAddPath);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }

        Path pointsOfInterestTemp4Path = new Path(tmpDirPath,
                                                  "points_of_interest_temp4");
        {
            ClusterAggBtsClusterJob job = new ClusterAggBtsClusterJob(conf);
            job.configure(new Path[] { pointsOffInterestTemp3Path,
                                       vectorBtsClusterPath },
                          pointsOfInterestTemp4Path);
            if (!job.waitForCompletion(true)) {
                throw new Exception("Failed to run " + job.getJobName());
            }
        }
    }
}
