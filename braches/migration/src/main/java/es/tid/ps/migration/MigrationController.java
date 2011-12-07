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
 * hadoop cluster to another. The requisites to used this class are: the version
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

	protected FileSystem hdfsSrc;
	protected FileSystem hdfsDst;

	/**
	 * Constuctor
	 * 
	 * @throws IOException
	 */
	public MigrationController() throws IOException {
		Configuration confSrc = new Configuration();
		confSrc.set(FS_DEFAULT_NAME, "hdfs://pshdp01.hi.inet:8011");
		Configuration confDst = new Configuration();
		confDst.set(FS_DEFAULT_NAME, "hdfs://pshdp01.hi.inet:8011");
		hdfsSrc = FileSystem.get(confSrc);
		hdfsDst = FileSystem.get(confDst);
	}

	/**
	 * Method that start the migration beteew the data from the souce to the
	 * destination
	 * 
	 * @throws IOException
	 */
	public void realizeMigration() throws IOException {
		String initialPath = "/user/rgc";
		Path path = new Path(initialPath);
		processDirectory(path);
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
	 * recives by param.
	 * 
	 * @param directory
	 *            tha path of directory that are will be processing
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
		try {
			out = hdfsDst.create(outputFile);

			in = hdfsSrc.open(inputFile);
			byte buffer[] = new byte[1024 * 1024 * 64];
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
