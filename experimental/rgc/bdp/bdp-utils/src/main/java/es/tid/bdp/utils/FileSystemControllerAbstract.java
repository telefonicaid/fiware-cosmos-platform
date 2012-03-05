package es.tid.bdp.utils;

import java.lang.reflect.Constructor;
import java.util.NoSuchElementException;
import java.util.Properties;

import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.parse.ParserAbstract;

public abstract class FileSystemControllerAbstract {

    public FileSystemControllerAbstract(
            final Properties proterties) {
    }

    /**
     * 
     * @param user
     * @param path
     * @return
     */
    public BdpFileDescriptor build(final String user, final String path) {
        String aux = path;
        while (aux.length() > 0) {
            try {
                return searchFileDescriptor(user, aux);
            } catch (NoSuchElementException e) {
                aux = aux.substring(0, aux.lastIndexOf("/"));
            }
        }
        try {
            return searchFileDescriptor(user, "/");
        } catch (NoSuchElementException e) {
            return createUnathorized();
        }
    }

    /**
     * 
     * @return
     */
    protected BdpFileDescriptor createUnathorized() {
        BdpFileDescriptor descriptor = new BdpFileDescriptor();
        descriptor.setCompressible(false);
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

    public static ParserAbstract createParser(final String className,
            final String pattern, final String attr) {
        try {
            @SuppressWarnings("unchecked")
            Class<ParserAbstract> klass = (Class<ParserAbstract>) Class
                    .forName(className);

            Constructor<ParserAbstract> constructor = klass.getConstructor(
                    String.class, String.class);

            return constructor.newInstance(pattern, attr);
        } catch (Exception e) {
            throw new RuntimeException();
        }
    }
}
