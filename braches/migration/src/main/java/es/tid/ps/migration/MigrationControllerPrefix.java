package es.tid.ps.migration;

import java.io.IOException;

import org.apache.hadoop.fs.Path;

/**
 * This class extends to @MigrationController and adds a prefix in all the files
 * that generate in the destination cluster
 * 
 * @author rgc
 * 
 */
public class MigrationControllerPrefix extends MigrationController {

	private final String prefix;
	private final static String PREFIX_PROPERTY = "prefix.value";
	/**
	 * Constuctor
	 * 
	 * @throws IOException
	 */
	public MigrationControllerPrefix() throws IOException {
		super();
		prefix = PropertiesPlaceHolder.getInstance().getProperty(PREFIX_PROPERTY, "");
	}

	/**
	 * Method that adds a prefix to all paths into the destination cluster
	 * 
	 * @param sourcePath
	 *            that path in the source cluster
	 * @return the path in the destination cluster
	 */
	protected Path generateOutputPath(Path sourcePath) {
		StringBuilder sb = new StringBuilder();
		Path aux = sourcePath;
		while (aux != null) {
			sb.insert(0, aux.getName());
			sb.insert(0, Path.SEPARATOR);
			aux = aux.getParent();
		}
		sb.insert(0, prefix);
		return new Path(sb.toString());
	}
}
