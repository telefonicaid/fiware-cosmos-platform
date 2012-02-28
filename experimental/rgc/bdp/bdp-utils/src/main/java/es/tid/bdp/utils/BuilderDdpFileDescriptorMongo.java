package es.tid.bdp.utils;

import java.util.NoSuchElementException;

import com.mongodb.BasicDBObject;
import com.mongodb.BasicDBObjectBuilder;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.Mongo;

import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.data.BdpFileDescriptor.BdpCompresion;

public class BuilderDdpFileDescriptorMongo extends
        BuilderDdpFileDescriptorAbstract {

    DBCollection collPath;

    public BuilderDdpFileDescriptorMongo(final String host, final int port,
            final String database, final String collection) {
        try {
            Mongo mongo = new Mongo(host, port);
            DB primary = mongo.getDB(database);
            collPath = primary.getCollection(collection);
        } catch (Exception e) {
            // TODO: handle exception
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * es.tid.bdp.utils.BuilderDdpFileDescriptorAbstract#searchFileDescriptor
     * (java.lang.String, java.lang.String)
     */
    @Override
    protected BdpFileDescriptor searchFileDescriptor(final String user,
            final String path) {

        BasicDBObject query = (BasicDBObject) BasicDBObjectBuilder.start()
                .add("$elemMatch", new BasicDBObject("user", user)).get();

        BasicDBObject result = (BasicDBObject) collPath.findOne(query);
        if (result != null) {
            return parsBasicDBObject(result);
        } else {
            throw new NoSuchElementException();
        }
    }

    /**
     * 
     * @param basicDBObject
     * @return
     */
    private BdpFileDescriptor parsBasicDBObject(BasicDBObject basicDBObject) {

        BdpFileDescriptor descriptor = new BdpFileDescriptor();

        descriptor.setSerializable(basicDBObject.getBoolean("isSerializable",
                false));
        descriptor.setReadable(basicDBObject.getBoolean("isReadble", false));
        descriptor.setWritable(basicDBObject.getBoolean("isWritable", false));

        BasicDBObject compressDBObject = (BasicDBObject) basicDBObject
                .get("compression");

        if (compressDBObject != null) {
            BdpCompresion compression = BdpCompresion.createInstance(
                    compressDBObject.getString("className"),
                    compressDBObject.getString("pattern"),
                    compressDBObject.getString("attr"));
            descriptor.setCompresion(compression);
        }

        return descriptor;

    }
}
