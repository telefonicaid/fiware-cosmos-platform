package es.tid.bdp.utils;

import es.tid.bdp.utils.data.BdpFileDescriptor;

public class BuilderDdpFileDescriptorTextAllow extends BuilderDdpFileDescriptorAbstract{

    @Override
    protected BdpFileDescriptor searchFileDescriptor(String user, String path) {
        BdpFileDescriptor descriptor = new BdpFileDescriptor();
        descriptor.setSerializable(false);
        descriptor.setReadable(true);
        descriptor.setWritable(true);
        return descriptor;
    }

}
