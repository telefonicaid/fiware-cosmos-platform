package org.apache.mahout.tid.common;

import java.io.*;

import java.util.Properties;

//SIMILARITY_EUCLIDEAN_DISTANCE
//SIMILARITY_COOCCURRENCE
//SIMILARITY_LOGLIKELIHOOD
//SIMILARITY_UNCENTERED_COSINE
//SIMILARITY_UNCENTERED_ZERO_ASSUMING_COSINE
//SIMILARITY_CITY_BLOCK
//SIMILARITY_TANIMOTO_COEFFICIENT
//SIMILARITY_PEARSON_CORRELATION
/**
 * 
 * @author jaume
 * 
 */
public class ReadJobConfig {

    /**
     * Configuration file
     */

    // Logger
    protected static String defaultMongoHost = "localhost";
    protected static int defaultMongoPort = 27017;

    protected static String defaultMongoDB = "recommender";

    protected static int defaultMaxRecommendations = 10;
    protected static String defaultTempPath = "/tmp/";

    public static String mongoHost = defaultMongoHost;
    public static int mongoPort = defaultMongoPort;
    public static String mongoDB = defaultMongoDB;

    public static String mongoCollectionEvents = "recommender";
    public static String mongoCollectionCatalog = "movie_catalog";
    public static String mongoCollectionRecomms = "recommendations";
    public static String mongoCollectionBL = "blacklist";

    public static String tempPath = "defaultTempPath";
    public static String OutputPath = "/tmp/output";

    public static int maxrecommendations = defaultMaxRecommendations;

    public String getMongoEvents() {
        return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionEvents;
    }

    public String getMongoCatalog() {
        return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionCatalog;
    }

    public String getMongoBL() {
        return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionBL;
    }

    public String getMongoRecomms() {
        return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
                + mongoCollectionRecomms;
    }

    public ReadJobConfig(String RecommenderProperties) {
        try {
            Properties properties = new Properties();
            try{
                properties.load(new FileInputStream(RecommenderProperties));
                String mongoCatalogProperty = properties.getProperty("MONGO_CATALOG");
                if (mongoCatalogProperty != null
                        && mongoCatalogProperty.length() > 0) {
                    try {
                        mongoCollectionCatalog = mongoCatalogProperty;
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }

                String mongoBLProperty = properties.getProperty("MONGO_BL");
                if (mongoBLProperty != null && mongoBLProperty.length() > 0) {
                    try {
                        mongoCollectionBL = mongoBLProperty;
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }

                String mongoEventsProperty = properties.getProperty("MONGO_EVENTS");
                if (mongoEventsProperty != null && mongoEventsProperty.length() > 0) {
                    try {
                        mongoCollectionEvents = mongoEventsProperty;
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }

                String mongoRecosProperty = properties.getProperty("MONGO_RECOMMS");
                if (mongoRecosProperty != null && mongoRecosProperty.length() > 0) {
                    try {
                        mongoCollectionRecomms = mongoRecosProperty;
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }

                String mongoHostProperty = properties.getProperty("MONGO_HOST");
                if (mongoHostProperty != null && mongoHostProperty.length() > 0) {
                    try {
                        mongoHost = mongoHostProperty;
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }

                String mongoPortProperty = properties.getProperty("MONGO_PORT");
                if (mongoPortProperty != null && mongoPortProperty.length() > 0) {
                    try {
                        mongoPort = Integer.parseInt(mongoPortProperty);
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }

                String mongoDBProperty = properties.getProperty("MONGO_DB");
                if (mongoDBProperty != null && mongoDBProperty.length() > 0) {
                    try {
                        mongoDB = mongoDBProperty;
                    } catch (Exception e) {
                        System.exit(0);
                    }
                }
            }
            catch(Exception e){
                //Sending Defaults
            }

        } catch (Exception e) {
            System.exit(0);
        }
    }
}
