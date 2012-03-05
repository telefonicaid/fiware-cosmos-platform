package es.tid.bdp.utils;

import java.util.Properties;

import es.tid.bdp.utils.data.BdpFileDescriptor;

public class BuilderDdpFileDescriptorTextAllow extends FileSystemControllerAbstract{

    public BuilderDdpFileDescriptorTextAllow(Properties proterties) {
        super(proterties);
    }

    @Override
    protected BdpFileDescriptor searchFileDescriptor(String user, String path) {
        BdpFileDescriptor descriptor = new BdpFileDescriptor();
        descriptor.setCompressible(false);
        descriptor.setReadable(true);
        descriptor.setWritable(true);
        return descriptor;
    }

}
