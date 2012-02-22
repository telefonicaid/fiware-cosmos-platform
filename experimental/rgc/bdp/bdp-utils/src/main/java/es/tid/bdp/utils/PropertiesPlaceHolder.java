package es.tid.bdp.utils;


import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

/**
 * Singleton that storage the properties and allow the access to the properties
 * into all of the application
 * 
 * @author rgc
 * 
 */
public class PropertiesPlaceHolder extends Properties {
    private static final long serialVersionUID = 2725239285541371712L;
    private static PropertiesPlaceHolder instance = null;

    private static final String CONFIGURATION_PATH_PROPERTY = "sftp-client.properties";

    /**
     * Private constructor
     * 
     * @throws FileNotFoundException
     * @throws IOException
     */
    private PropertiesPlaceHolder(String configFile)
            throws FileNotFoundException, IOException {
        this.load(new FileInputStream(configFile));
    }

    /**
     * Static method for access to the singleton object
     * 
     * @return the singleton PropertiesPlaceHolder
     * @throws FileNotFoundException
     *             if the file does not exist, is a directory rather than a
     *             regular file, or for some other reason cannot be opened for
     *             reading.
     * @throws IOException
     *             if the file does not exist, is a directory rather than a
     *             regular file, or for some other reason cannot be opened for
     *             reading.
     */
    public static PropertiesPlaceHolder getInstance()
            throws FileNotFoundException, IOException {
        if (instance == null) {
            synchronized (PropertiesPlaceHolder.class) {
                if (instance == null) {
                    String configFile = System.getProperty(CONFIGURATION_PATH_PROPERTY);
                    if (configFile.isEmpty()){
                        throw new IllegalArgumentException("Config file is not configurate");
                    }
                    instance = new PropertiesPlaceHolder(configFile);
                }
            }
        }
        return instance;
    }

    /**
     * Method that generates the singleton instance. If the instance has been
     * initialized before it throws an exception
     * 
     * @param configFile
     *            the path of configuration file
     * @return the singleton PropertiesPlaceHolder
     * @throws FileNotFoundException
     *             if the file does not exist, is a directory rather than a
     *             regular file, or for some other reason cannot be opened for
     *             reading.
     * @throws IOException
     *             if the file does not exist, is a directory rather than a
     *             regular file, or for some other reason cannot be opened for
     *             reading.
     * @throws RuntimeException
     *             if the exception has been initialized
     */
    public static PropertiesPlaceHolder createInstance(String configFile)
            throws FileNotFoundException, IOException {
        if (instance == null) {
            synchronized (PropertiesPlaceHolder.class) {
                if (instance == null) {
                    instance = new PropertiesPlaceHolder(configFile);
                    return instance;
                }
            }
        }
        throw new RuntimeException("Inicializate Singleton Instance");
    }

    /**
     * Searches for the property with the specified key in this property list
     * and it parses the value to Integer
     * 
     * @param key
     *            the property key.
     * @return the value in this property list with the specified key value in a
     *         Integer.
     * @exception NumberFormatException
     *                - if the string does not contain a parsable integer.
     */
    public Integer getPropertyInt(String key) {
        return Integer.parseInt(this.getProperty(key));
    }

    /**
     * Searches for the property with the specified key in this property list
     * and it parses the value to Integer. If the value is not into the
     * properties, or it is not a number return the default value.
     * 
     * @param key
     *            the property key.
     * @return the value in this property list with the specified key value in a
     *         Integer.
     */
    public Integer getPropertyInt(String key, int defaultValue) {
        try {
            return Integer.parseInt(this.getProperty(key));
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }
     
    /**
     * Searches for the property with the specified key in this property list
     * and it parses the value to boolean
     * 
     * @param key
     *            the property key.
     * @return the value in this property list with the specified key value into a
     *         Boolean.
     */
    public Boolean getPropertyBool(String key) {
        return Boolean.parseBoolean(this.getProperty(key));
    }
}