namespace java es.tid.bdp.platform.cluster.server
namespace py cluster_api

exception TransferException {
    1: i32 errorCode,
    2: string errorMsg
}

struct ClusterJobStatus {
    1: i32 id
    2: string username
}

service Cluster {
    void copyToHdfs(1:string srcPath, 2:string destPath) 
            throws (1: TransferException ex),

    void runJob(1:string jarPath, 2:string inputPath, 3:string outputPath,
                4:string mongoUrl),

    list<ClusterJobStatus> getRunningJobs(1:string user)
            throws (1: TransferException ex),
}
