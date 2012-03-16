/**
 * 
 */
package es.tid.ps.migration;

import java.io.IOException;

/**
 * Main class that makes the migration of the data from a source Hadoop cluster
 * to a destination Hadoop cluster
 * 
 * @author rgc
 * 
 */
public class MigrationMain {

	/**
	 * Main method
	 * 
	 * @param args
	 *            this function does not need arguments for running
	 * @throws IOException
	 */
	public static void main(String[] args) throws IOException {
		MigrationController mc = new MigrationControllerPrefix();
		mc.realizeMigration();
	}
}
