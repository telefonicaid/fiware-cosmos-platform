package es.tid.bdp.recomms.common;

import java.io.*;
import java.util.Properties;

/**
 * 
 * @author jaume
 * 
 */
public abstract class ReadJobConfig {
    protected static String defaultMongoHost = "localhost";
    protected static String defaultMongoDB = "recommender";
    protected static String defaultTempPath = "/tmp/";
    protected static int defaultMongoPort = 27017;
    protected static int defaultMaxRecommendations = 10;
    protected static String defaultMongoEventsCollection = "recommender";
    protected static String defaultMongoCatalogCollection = "movie_catalog";
    protected static String defaultMongoRecommsCollection = "recommendations";
    protected static String defaultMongoBLCollection = "blacklist";
    protected static final String mongoURL = "mongodb://";

    private static int mongoPort = defaultMongoPort;
    private static String mongoDB = defaultMongoDB;
    private static String mongoHost = defaultMongoHost;
    private static String mongoCollectionEvents = defaultMongoEventsCollection;
    private static String mongoCollectionCatalog = defaultMongoCatalogCollection;
    private static String mongoCollectionRecomms = defaultMongoRecommsCollection;
    private static String mongoCollectionBL = defaultMongoBLCollection;

    public static String getMongoEvents() {
        return mongoURL + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionEvents;
    }

    public static String getMongoCatalog() {
        return mongoURL + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionCatalog;
    }

    public static String getMongoBL() {
        return mongoURL + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionBL;
    }

    public static String getMongoRecomms() {
        return mongoURL + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionRecomms;
    }

    public static void readProperties(String RecommenderProperties)
            throws Exception {
        Properties properties = new Properties();
        try {
            properties.load(new FileInputStream(RecommenderProperties));
            String mongoCatalogProperty = properties
                    .getProperty("MONGO_CATALOG");
            if (mongoCatalogProperty != null
                    && mongoCatalogProperty.length() > 0) {
                mongoCollectionCatalog = mongoCatalogProperty;
            }

            String mongoBLProperty = properties.getProperty("MONGO_BL");
            if (mongoBLProperty != null && mongoBLProperty.length() > 0) {
                mongoCollectionBL = mongoBLProperty;
            }

            String mongoEventsProperty = properties.getProperty("MONGO_EVENTS");
            if (mongoEventsProperty != null && mongoEventsProperty.length() > 0) {
                mongoCollectionEvents = mongoEventsProperty;
            }

            String mongoRecosProperty = properties.getProperty("MONGO_RECOMMS");
            if (mongoRecosProperty != null && mongoRecosProperty.length() > 0) {
                mongoCollectionRecomms = mongoRecosProperty;
            }

            String mongoHostProperty = properties.getProperty("MONGO_HOST");
            if (mongoHostProperty != null && mongoHostProperty.length() > 0) {
                mongoHost = mongoHostProperty;
            }

            String mongoPortProperty = properties.getProperty("MONGO_PORT");
            if (mongoPortProperty != null && mongoPortProperty.length() > 0) {
                mongoPort = Integer.parseInt(mongoPortProperty);
            }

            String mongoDBProperty = properties.getProperty("MONGO_DB");
            if (mongoDBProperty != null && mongoDBProperty.length() > 0) {
                mongoDB = mongoDBProperty;
            }
        } catch (Exception e) {
            throw new Exception(e.getMessage());
        }
    }
}
