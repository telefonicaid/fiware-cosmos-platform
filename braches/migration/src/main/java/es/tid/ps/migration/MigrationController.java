package es.tid.ps.migration;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

/**
 * This abstract class provides the functionality for loading data from one
 * Hadoop cluster to another. The requisites to used this class are: the version
 * of the cluster are the same or a version or at least compatible versions.
 * 
 * The class extends this need to implement the functionality for defining how
 * is the transformation of the path from the source to the destination.
 * 
 * @author rgc
 * 
 */
public abstract class MigrationController {

	private final static String FS_DEFAULT_NAME = "fs.default.name";

	private final static String FS_NAME_SOURCE = "fs.name.source";
	private final static String FS_NAME_DESTINATION = "fs.name.destination";
	private final static String OVERWRITE_FILES = "files.overwrite";
	private static final String INITIAL_PATH = "initial.path";

	private final static String DEFAULT_INITIAL_PATH = "/";

	private final static int DATA_BLOCK_SIZE = 1024 * 1024 * 64;

	protected final FileSystem hdfsSrc;
	protected final FileSystem hdfsDst;
	protected final String initialPath;
	protected final Boolean overwrite;

	/**
	 * Constuctor
	 * 
	 * @throws IOException
	 */
	public MigrationController() throws IOException {

		PropertiesPlaceHolder properties = PropertiesPlaceHolder.getInstance();

		Configuration confSrc = new Configuration();
		confSrc.set(FS_DEFAULT_NAME, properties.getProperty(FS_NAME_SOURCE));
		Configuration confDst = new Configuration();
		confDst.set(FS_DEFAULT_NAME,
				properties.getProperty(FS_NAME_DESTINATION));

		overwrite = Boolean.getBoolean(properties.getProperty(OVERWRITE_FILES));

		hdfsSrc = FileSystem.get(confSrc);
		hdfsDst = FileSystem.get(confDst);

		initialPath = properties
				.getProperty(INITIAL_PATH, DEFAULT_INITIAL_PATH);
	}

	/**
	 * Method that start the migration beteew the data from the souce to the
	 * destination
	 * 
	 * @throws IOException
	 */
	public void realizeMigration() throws IOException {
		processDirectory(new Path(initialPath));
	}

	/**
	 * Abstract method that transforms the path source in a path destination.
	 * 
	 * @param sourcePath
	 *            that path in the source cluster
	 * @return the path in the destination cluster
	 */
	protected abstract Path generateOutputPath(Path sourcePath);

	/**
	 * This method processes all the directories and files from a directory that
	 * Receives by parameter.
	 * 
	 * @param directory
	 *            path of directory that are will be processing
	 * @throws IOException
	 */
	private void processDirectory(Path directory) throws IOException {
		System.out.println(directory);
		hdfsDst.mkdirs(generateOutputPath(directory));
		FileStatus[] elements = hdfsSrc.listStatus(directory);
		for (int i = 0; i < elements.length; ++i) {
			if (elements[i].isDir()) {
				processDirectory(elements[i].getPath());
			} else {
				Path inputFile = elements[i].getPath();
				Path outputFile = generateOutputPath(elements[i].getPath());
				copy(inputFile, outputFile);
			}
		}
	}

	/**
	 * This method copies a file from the cluster source to the cluster
	 * destination
	 * 
	 * @param inputFile
	 *            the path of the file in the cluster source
	 * @param outputFile
	 *            the path of the file in the cluster destination
	 * @throws IOException
	 */
	private void copy(Path inputFile, Path outputFile) throws IOException {
		System.out.println("copy from " + inputFile + " to " + outputFile);
		FSDataOutputStream out = null;
		FSDataInputStream in = null;

		if (!overwrite && hdfsDst.exists(outputFile)) {
			return;
		}
		
		try {
			out = hdfsDst.create(outputFile);
			in = hdfsSrc.open(inputFile);
			byte buffer[] = new byte[DATA_BLOCK_SIZE];
			int bytesRead = 0;
			while ((bytesRead = in.read(buffer)) != -1) {
				out.write(buffer, 0, bytesRead);
			}
		} finally {
			if (in != null) {
				in.close();
			}
			if (out != null) {
				out.close();
			}
		}
	}
}
