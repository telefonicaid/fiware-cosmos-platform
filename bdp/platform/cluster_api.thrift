namespace py cluster_api
namespace java clusterapi

exception TransferException {
    1: i32 errorCode,
    2: string errorMsg
}

service Cluster {
    void copyToHdfs(1:string srcPath, 2:string destPath) 
        throws (1: TransferException ex),
}
