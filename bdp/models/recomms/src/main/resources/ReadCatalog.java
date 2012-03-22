package org.apache.mahout.ps20.impl.recommender;

import java.io.*;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Properties;

import org.apache.log4j.PropertyConfigurator;
import org.bson.types.ObjectId;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.mongodb.BasicDBObject;
import com.mongodb.DB;
import com.mongodb.DBCollection;
import com.mongodb.Mongo;


public class ReadCatalog {
  

  /**
   * Configuration file
   */
  private static final String RECOMMENDER_PROPERTIES = "/home/jaume/mahout/recommender.properties";
  
  /**
   * Configuration file
   */
  private static final String LOG4J_PROPERTIES = "/home/jaume/mahout/log4j.properties";

  // Logger
  private static final Logger log = LoggerFactory.getLogger(ReadCatalog.class);
  protected static String defaultMongoHost = "localhost";
  protected static int defaultMongoPort = 27017;
  protected static String defaultMongoDB = "movie_catalog";
  protected static boolean defaultMongoAuth = false;
  protected static String defaultMongoUsername = "recommender";
  protected static String defaultMongoPassword = "recommender";
  protected static String defaultMongoCollection = "items";
  protected static boolean defaultMongoManage = true;
  protected static String defaultMongoUserID = "user_id";
  protected static String defaultMongoItemID = "item_id";
  protected static String defaultMongoPreference = "preference";
  protected static boolean defaultMongoFinalRemove = false;
  protected static SimpleDateFormat defaultDateFormat = new SimpleDateFormat("EE MMM dd yyyy HH:mm:ss 'GMT'Z (zzz)");
  protected static double defaultUserThreshold = 0.8;
  protected static int defaultNeighborsNumber = 10;
  protected static int defaultMaxRecommendations = 10;
  protected static String defaultSimilarityMeasure = "euclidean";
  protected static String defaultNeighborhoodType = "nearest";
  protected static String mongoHost = defaultMongoHost;
  protected static int mongoPort = defaultMongoPort;
  protected static String mongoDB = defaultMongoDB;
  protected static boolean mongoAuth = defaultMongoAuth;
  protected static String mongoUsername = defaultMongoUsername;
  protected static String mongoPassword = defaultMongoPassword;
  protected static String mongoCollection = defaultMongoCollection;
  protected static boolean mongoManage = defaultMongoManage;
  protected static String mongoUserID = defaultMongoUserID;
  protected static String mongoItemID = defaultMongoItemID;
  protected static String mongoPreference = defaultMongoPreference;
  protected static boolean mongoFinalRemove = defaultMongoFinalRemove;
  protected static SimpleDateFormat dateFormat = defaultDateFormat;
  protected static double userThreshold = defaultUserThreshold;
  protected static int neighborsNumber = defaultNeighborsNumber;
  protected static int maxRecommendations = defaultMaxRecommendations;
  protected static String neighborhoodType = defaultNeighborhoodType;
  protected static String similarityMeasure = defaultSimilarityMeasure;
  protected static String catalogsFile = "/home/jaume/mahout/movies.dat";
  protected static boolean itemIsObject = false;
  public static void main (String args[]) {
    // Starts service
    try {
      log.info("Starting Tastenet item recommender server");
      getParameters();
    } catch (Exception e) {
      log.error("Error while getting Parameteres.", e);
    }
    clean();
    insertData();
  }
  
  private static void getParameters() {
    try {
      log.info("Reading parameters");
      Properties properties = new Properties();
      properties.load(new FileInputStream(RECOMMENDER_PROPERTIES));
      PropertyConfigurator.configure(new File(LOG4J_PROPERTIES).getAbsolutePath());
      
      String mongoHostProperty = properties.getProperty("MONGO_HOST");
      if (mongoHostProperty != null && mongoHostProperty.length() > 0) {
        try {
          mongoHost = mongoHostProperty;
        } catch (Exception e) {
          log.error("Property [MONGO_HOST] on properties file has an invalid value("
          + mongoHostProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_HOST read from configuration file: " + mongoHost);
      
      String mongoPortProperty = properties.getProperty("MONGO_PORT");
      if (mongoPortProperty != null && mongoPortProperty.length() > 0) {
        try {
          mongoPort = Integer.parseInt(mongoPortProperty);
        } catch (Exception e) {
          log.error("Property [MONGO_PORT] on properties file has an invalid value("
          + mongoPortProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_PORT read from configuration file: " + mongoPort);
      
      String mongoDBProperty = properties.getProperty("MONGO_DB");
      if (mongoDBProperty != null && mongoDBProperty.length() > 0) {
        try {
          mongoDB = mongoDBProperty;
        } catch (Exception e) {
          log.error("Property [MONGO_DB] on properties file has an invalid value("
          + mongoDBProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_DB read from configuration file: " + mongoDB);
      
      String mongoCollectionProperty = properties.getProperty("MONGO_ITEMS_COLLECTION");
      if (mongoCollectionProperty != null && mongoCollectionProperty.length() > 0) {
        try {
          mongoCollection = mongoCollectionProperty;
        } catch (Exception e) {
          log.error("Property [MONGO_COLLECTION] on properties file has an invalid value("
          + mongoCollectionProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_COLLECTION read from configuration file: " + mongoCollection);
      
      String mongoManageProperty = properties.getProperty("MONGO_MANAGE");
      if (mongoManageProperty != null && mongoManageProperty.length() > 0) {
        try {
          mongoManage = Boolean.parseBoolean(mongoManageProperty);
        } catch (Exception e) {
          log.error("Property [MONGO_MANAGE] on properties file has an invalid value("
          + mongoManageProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_MANAGE read from configuration file: " + mongoManage);
      
      String mongoFinalRemoveProperty = properties.getProperty("MONGO_FINAL_REMOVE");
      if (mongoFinalRemoveProperty != null && mongoFinalRemoveProperty.length() > 0) {
        try {
          mongoFinalRemove = Boolean.parseBoolean(mongoFinalRemoveProperty);
        } catch (Exception e) {
          log.error("Property [MONGO_FINAL_REMOVE] on properties file has an invalid value("
          + mongoFinalRemoveProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_FINAL_REMOVE read from configuration file: " + mongoFinalRemove);
      
      String dateFormatProperty = properties.getProperty("DATE_FORMAT");
      if (dateFormatProperty != null && dateFormatProperty.length() > 0) {
        try {
          if (dateFormatProperty.equals("NULL")) {
            dateFormat = null;
          } else {
            dateFormat = new SimpleDateFormat(dateFormatProperty);
          }
        } catch (Exception e) {
          log.error("Property [DATE_FORMAT] on properties file has an invalid value("
          + dateFormatProperty + ")");
          System.exit(0);
        }
      }
      log.info("DATE_FORMAT read from configuration file: " + (dateFormat == null ? "NULL" : dateFormat.toPattern()));
      
      String mongoAuthProperty = properties.getProperty("MONGO_AUTH");
      if (mongoAuthProperty != null && mongoAuthProperty.length() > 0) {
        try {
          mongoAuth = Boolean.parseBoolean(mongoAuthProperty);
        } catch (Exception e) {
          log.error("Property [MONGO_AUTH] on properties file has an invalid value("
          + mongoAuthProperty + ")");
          System.exit(0);
        }
      }
      log.info("MONGO_AUTH read from configuration file: " + mongoAuth);
      
      if (mongoAuth) {
        String mongoUsernameProperty = properties.getProperty("MONGO_USERNAME");
        if (mongoUsernameProperty != null && mongoUsernameProperty.length() > 0) {
          try {
            mongoUsername = mongoUsernameProperty;
          } catch (Exception e) {
            log.error("Property [MONGO_USERNAME] on properties file has an invalid value("
            + mongoUsernameProperty + ")");
            System.exit(0);
          }
        }
        log.info("MONGO_USERNAME read from configuration file: " + mongoUsername);
        
        String mongoPasswordProperty = properties.getProperty("MONGO_PASSWORD");
        if (mongoPasswordProperty != null && mongoPasswordProperty.length() > 0) {
          try {
            mongoPassword = mongoPasswordProperty;
          } catch (Exception e) {
            log.error("Property [MONGO_PASSWORD] on properties file has an invalid value("
            + mongoPasswordProperty + ")");
            System.exit(0);
          }
        }
        log.info("MONGO_PASSWORD read from configuration file: " + mongoPassword);
      }   
      
      String catalogsfile = properties.getProperty("CATALOG_FILE");
      if (catalogsfile != null && catalogsfile.length() > 0) {
          try {
              catalogsFile = catalogsfile;
            } catch (Exception e) {
              log.error("Property [CATALOG_FILE] on properties file has an invalid value(" + catalogsfile + ")");
              System.exit(0);
            }
          }
      
    } catch (Exception e) {
      log.error("Error while starting recommender.", e);
    }
  }
  
  /************************************************
   ******************** UTILS *******************
   ************************************************/
  private static void insertData() {
    try {
      itemIsObject = false;

      Mongo mongoDDBB = new Mongo(mongoHost , mongoPort);
      DB db = mongoDDBB.getDB(mongoDB);
      
      DBCollection collection = db.getCollection(mongoCollection);
      
      FileInputStream fstream = new FileInputStream(catalogsFile);
      DataInputStream in = new DataInputStream(fstream);
      
      BufferedReader br = new BufferedReader(new InputStreamReader(in));
      String strLine;
      
      while ((strLine = br.readLine()) != null)
      {
    	  String[] splits = strLine.split("::");
    	  ArrayList<String> cats =new ArrayList<String>();
    	  String[] categories = splits[3].split("[|]"); 
    	  for(int i = 0; i < categories.length; i++) 
    		  cats.add(categories[i]);
    	  
          addMongoItem(collection, splits[0], splits[1], splits[2], cats);
      }
    	  //Close the input stream
   	  in.close();

///TIMESTAMPS TO CONSIDER

    } catch(Exception e) {
      System.out.println("[ERROR] Creating test data in collection: " + mongoCollection);
    }
  }
  
  private static void clean() {
    try {
      Mongo mongoDDBB = new Mongo(mongoHost , mongoPort);
      DB db = mongoDDBB.getDB(mongoDB);
      DBCollection collection = db.getCollection(mongoCollection);
      collection.remove(new BasicDBObject());
    } catch(Exception e) {
      System.out.println("[ERROR] Creating test data in collection: " + mongoCollection);
    }
  }
  
  private static void addMongoItem(DBCollection collection,String itemID, String Name, String Year, ArrayList<String> Categories) {
    
    BasicDBObject item = new BasicDBObject();
    Object itemId = (itemIsObject ? new ObjectId(itemID) : itemID);

    item.put("item_id", itemId);
    item.put("name", Name);    
    item.put("year", Year);
    item.put("categories", Categories);    
    item.put("created_at", new Date());
    System.out.println("OK. item: " + item.toString());
    collection.insert(item);
  }

}
