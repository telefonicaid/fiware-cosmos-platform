package es.tid.ps.migration;

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

	private static final String CONFIGURATION_PATH = "migration.properties";

	/**
	 * Private constructor
	 * 
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	private PropertiesPlaceHolder() throws FileNotFoundException, IOException {
		this.load(new FileInputStream(CONFIGURATION_PATH));
	}

	/**
	 * Static method for access to the singleton object
	 * 
	 * @return the singleton PropertiesPlaceHolder
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	synchronized public static PropertiesPlaceHolder getInstance()
			throws FileNotFoundException, IOException {
		if (instance == null) {
			instance = new PropertiesPlaceHolder();
		}
		return instance;
	}
}
