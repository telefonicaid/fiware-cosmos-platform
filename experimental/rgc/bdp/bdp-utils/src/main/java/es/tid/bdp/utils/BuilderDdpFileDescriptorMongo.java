package es.tid.bdp.utils;

import java.util.NoSuchElementException;

import com.mongodb.BasicDBObject;
import com.mongodb.BasicDBObjectBuilder;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.DBPort;
import com.mongodb.Mongo;

import es.tid.bdp.utils.data.BdpFileDescriptor;
import es.tid.bdp.utils.parse.ParserAbstract;

public class BuilderDdpFileDescriptorMongo extends
        BuilderDdpFileDescriptorAbstract {
    public static final String DESCRIPTOR_MONGODB_HOST = "description.mongodb.host";
    public static final String DESCRIPTOR_MONGODB_PORT = "description.mongodb.port";
    public static final String DESCRIPTOR_MONGODB_DB = "description.mongodb.db";
    public static final String DESCRIPTOR_MONGODB_COLLECTION = "description.mongodb.collection";

    private DBCollection collPath;

    public BuilderDdpFileDescriptorMongo(PropertiesPlaceHolder properties) {
        super(properties);
        try {
            Mongo mongo = new Mongo(
                    properties.getProperty(DESCRIPTOR_MONGODB_HOST),
                    properties.getPropertyInt(DESCRIPTOR_MONGODB_DB,
                            DBPort.PORT));
            DB primary = mongo.getDB(properties
                    .getProperty(DESCRIPTOR_MONGODB_DB));
            collPath = primary.getCollection(properties
                    .getProperty(DESCRIPTOR_MONGODB_COLLECTION));
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

        BasicDBObject query = (BasicDBObject) BasicDBObjectBuilder.start().add("_id", path)
                .add("users." + user, new BasicDBObject("$exists", true)).get();

        BasicDBObject result = (BasicDBObject) ((BasicDBObject) collPath.findOne(query).get("users")).get(user);
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

        descriptor.setCompressible(basicDBObject.getBoolean("isCompressible",
                false));
        descriptor.setReadable(basicDBObject.getBoolean("isReadble", false));
        descriptor.setWritable(basicDBObject.getBoolean("isWritable", false));

        BasicDBObject parserDBObject = (BasicDBObject) basicDBObject
                .get("paser");

        if (parserDBObject != null) {

            ParserAbstract parser = this.createParser(
                    parserDBObject.getString("className"),
                    parserDBObject.getString("pattern"),
                    parserDBObject.getString("attr"));
            descriptor.setParser(parser);
        }

        return descriptor;

    }
}
