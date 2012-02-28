package es.tid.bdp.utils;

import java.util.NoSuchElementException;

import es.tid.bdp.utils.data.BdpFileDescriptor;

public abstract class BuilderDdpFileDescriptorAbstract {

    /**
     * 
     * @param user
     * @param path
     * @return
     */
    public BdpFileDescriptor build(final String user, final String path) {
        String aux = path;
        while (!aux.isEmpty()) {
            try {
                return searchFileDescriptor(user, aux);
            } catch (NoSuchElementException e) {
                aux = aux.substring(0, aux.lastIndexOf("/"));
            }
        }
        return createUnathorized();
    }

    /**
     * 
     * @return
     */
    protected BdpFileDescriptor createUnathorized() {
        BdpFileDescriptor descriptor = new BdpFileDescriptor();
        descriptor.setSerializable(false);
        descriptor.setReadable(false);
        descriptor.setWritable(false);

        return descriptor;
    }

    /**
     * 
     * @param user
     * @param path
     * @return
     */
    protected abstract BdpFileDescriptor searchFileDescriptor(
            final String user, final String path);
}
