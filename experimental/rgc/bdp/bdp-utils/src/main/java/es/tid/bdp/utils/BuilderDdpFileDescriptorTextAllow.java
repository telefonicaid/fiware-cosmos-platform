package es.tid.bdp.utils;

import es.tid.bdp.utils.data.BdpFileDescriptor;

public class BuilderDdpFileDescriptorTextAllow extends BuilderDdpFileDescriptorAbstract{

    public BuilderDdpFileDescriptorTextAllow(PropertiesPlaceHolder proterties) {
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
