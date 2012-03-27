namespace java es.tid.bdp.platform.cluster.server
namespace py cluster_api

enum ClusterErrorCode {
    FILE_COPY_FAILED = 1,
    RUN_JOB_FAILED = 2,
    INVALID_JOB_ID = 3
}

exception TransferException {
    1: ClusterErrorCode errorCode,
    2: string errorMsg
}

enum ClusterJobStatus {
    NOT_STARTED = 1,
    RUNNING = 2,
    SUCCESSFUL = 3,
    FAILED = 4
}

struct ClusterJobResult {
    1: ClusterJobStatus status
    2: TransferException reason
} 

service Cluster {
    void copyToHdfs(1:string srcPath, 2:string destPath) 
            throws (1: TransferException ex),

    string runJob(1:string jarPath, 2:string inputPath, 3:string outputPath,
                  4:string mongoUrl) throws (1: TransferException ex),

    ClusterJobResult getJobResult(1:string jobId)
            throws (1: TransferException ex),
}
