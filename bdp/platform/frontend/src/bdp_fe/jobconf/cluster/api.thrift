namespace py cluster_api

exception TransferException {
    1: i32 error_code,
    2: string error_msg
}

service Cluster {
    void copyToHdfs(1:string src_path, 2:string dest_path) 
        throws (1: TransferException ex),
}
