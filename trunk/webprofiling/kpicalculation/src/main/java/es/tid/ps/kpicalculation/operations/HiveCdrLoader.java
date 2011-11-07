package es.tid.ps.kpicalculation.operations;

import java.io.IOException;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.text.MessageFormat;
import java.util.logging.Logger;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.PathFilter;

/**
 * Class that loads the filtered data of CDR's into hive. TARGET_TABLE contains
 * the fixed name of the table where the data will be loaded.
 * 
 * @author javierb
 * 
 */
public class HiveCdrLoader implements ICdrLoader {
    //
    private static String TARGET_TABLE = "PAGE_VIEWS";

    private Statement stmt;

    private static String driverName = "org.apache.hadoop.hive.jdbc.HiveDriver";
    private static Logger logger;

    private Configuration conf;

    /**
     * Constructor method that receives the current context configuration
     * 
     * @param configuration
     *            current configuration
     * 
     */
    public HiveCdrLoader(Configuration configuration) {
        logger = Logger.getLogger(HiveCdrLoader.class.getName());
        conf = configuration;
        try {
            Class.forName(driverName);
            Connection con = DriverManager.getConnection("jdbc:hive://pshdp02:10000", "", "");
            stmt = con.createStatement();
        } catch (SQLException e) {
            // TODO Auto-generated catch block
            logger.severe(e.toString());
        } catch (ClassNotFoundException e) {
            // TODO Auto-generated catch block
            logger.severe(e.toString());
        }

    }

    /**
     * Loads all the temporal files generated during the filtering phase into
     * hive's table. And removes the temporal folder when the process finishes
     * 
     * @param folder
     *            path of the folder
     * 
     */
    @Override
    public int load(String folder) {
        try {
            FileSystem fs = FileSystem.get(conf);
            FileStatus[] status = fs.listStatus(new Path(conf.get("kpicalculation.temp.path")), new PathFilter() {
                
                @Override
                public boolean accept(Path arg0) {
                    if( arg0.getName().startsWith("part-r-"))
                        return true;
                    return false;
                }
            });
            
            for (int i = 0; i < status.length; i++) {
                System.out.println(status[i].getPath().getName());
                String sql = MessageFormat.format("LOAD DATA INPATH \"{0}\" OVERWRITE INTO TABLE {1}",
                        status[i].getPath(), TARGET_TABLE);
                System.out.println(sql);
                stmt.executeQuery(sql);
                
            }
            fs.delete(new Path(conf.get("kpicalculation.temp.path")), true);
            
            
        } catch (IOException e) {
            e.printStackTrace();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return 0;
    }

}
