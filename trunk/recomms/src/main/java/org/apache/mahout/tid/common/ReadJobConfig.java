package org.apache.mahout.tid.common;

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

	public static int mongoPort = defaultMongoPort;
	public static String mongoDB = defaultMongoDB;
	public static String mongoHost = defaultMongoHost;
	public static String mongoCollectionEvents = defaultMongoEventsCollection;
	public static String mongoCollectionCatalog = defaultMongoCatalogCollection;
	public static String mongoCollectionRecomms = defaultMongoRecommsCollection;
	public static String mongoCollectionBL = defaultMongoBLCollection;

	public static String getMongoEvents() {
		return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
				+ mongoCollectionEvents;
	}

	public static String getMongoCatalog() {
		return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
				+ mongoCollectionCatalog;
	}

	public static String getMongoBL() {
		return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
				+ mongoCollectionBL;
	}

	public static String getMongoRecomms() {
		return "mongodb://" + mongoHost + ':' + mongoPort + "/" + mongoDB + "."
				+ mongoCollectionRecomms;
	}

	public static void readProperties(String RecommenderProperties) throws FileNotFoundException {
		Properties properties = new Properties();
		try {
			properties.load(new FileInputStream(RecommenderProperties));
			String mongoCatalogProperty = properties
					.getProperty("MONGO_CATALOG");
			if (mongoCatalogProperty != null
					&& mongoCatalogProperty.length() > 0) {
				try {
					mongoCollectionCatalog = mongoCatalogProperty;
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}

			String mongoBLProperty = properties.getProperty("MONGO_BL");
			if (mongoBLProperty != null && mongoBLProperty.length() > 0) {
				try {
					mongoCollectionBL = mongoBLProperty;
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}

			String mongoEventsProperty = properties.getProperty("MONGO_EVENTS");
			if (mongoEventsProperty != null && mongoEventsProperty.length() > 0) {
				try {
					mongoCollectionEvents = mongoEventsProperty;
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}

			String mongoRecosProperty = properties.getProperty("MONGO_RECOMMS");
			if (mongoRecosProperty != null && mongoRecosProperty.length() > 0) {
				try {
					mongoCollectionRecomms = mongoRecosProperty;
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}

			String mongoHostProperty = properties.getProperty("MONGO_HOST");
			if (mongoHostProperty != null && mongoHostProperty.length() > 0) {
				try {
					mongoHost = mongoHostProperty;
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}

			String mongoPortProperty = properties.getProperty("MONGO_PORT");
			if (mongoPortProperty != null && mongoPortProperty.length() > 0) {
				try {
					mongoPort = Integer.parseInt(mongoPortProperty);
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}

			String mongoDBProperty = properties.getProperty("MONGO_DB");
			if (mongoDBProperty != null && mongoDBProperty.length() > 0) {
				try {
					mongoDB = mongoDBProperty;
				} catch (Exception e) {
					throw new NullPointerException();
				}
			}
		} catch (Exception e) {
			throw new FileNotFoundException();
		}
	}
}
