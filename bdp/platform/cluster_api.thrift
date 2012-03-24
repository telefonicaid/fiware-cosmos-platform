namespace java es.tid.bdp.platform.cluster.server
namespace py cluster_api

exception TransferException {
    1: i32 errorCode,
    2: string errorMsg
}

enum ClusterJobStatus {
    RUNNING = 1,
    SUCCESSFUL = 2,
    FAILED = 3
}

service Cluster {
    void copyToHdfs(1:string srcPath, 2:string destPath) 
            throws (1: TransferException ex),

    string runJob(1:string jarPath, 2:string inputPath, 3:string outputPath,
               4:string mongoUrl),

    ClusterJobStatus getJobStatus(1:string jobId)
}
