package es.tid.ps.kpicalculation;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Date;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

/**
 * This class performs the webprofiling processing of the data received from
 * dpis and calculates different kpis of about that data. The process is a chain
 * of map & reduces operations that will generate the final results.
 * 
 * TODO: At the moment due to problems using the jdbc-hive connector directly,
 * the hive server must be used. This should be reviewed in the future. All the
 * queries are added sequentially and will need to be extracted to a properties
 * file in order to modify the process without the need of recompiling the
 * project
 */
public class KpiMain extends Configured implements Tool {
    private static String driverName = "org.apache.hadoop.hive.jdbc.HiveDriver";

    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(), new KpiMain(), args);
        System.exit(res);
    }

    public int run(String[] args) throws Exception {
        Path inputPath = new Path(args[0]);

        Path tmpPath = new Path("/user/javierb/tmp." + Long.toString(new Date().getTime()));

        Configuration conf = getConf();
        conf.set("mapred.reduce.tasks", "1");

        Job wpCleanerJob = new Job(conf, "Web Profiling ...");
        wpCleanerJob.setJarByClass(KpiMain.class);
        wpCleanerJob.setMapperClass(KpiCleanerMapper.class);
        wpCleanerJob.setReducerClass(KpiCleanerReducer.class);
        wpCleanerJob.setInputFormatClass(TextInputFormat.class);
        wpCleanerJob.setOutputKeyClass(Text.class);
        wpCleanerJob.setMapOutputValueClass(Text.class);
        wpCleanerJob.setMapOutputKeyClass(LongWritable.class);
        wpCleanerJob.setOutputValueClass(Text.class);
        wpCleanerJob.setOutputFormatClass(TextOutputFormat.class);

        FileInputFormat.addInputPath(wpCleanerJob, inputPath);
        FileOutputFormat.setOutputPath(wpCleanerJob, tmpPath);

        // TODO(rgc): check if this funtionality is correct
        // FileSystem hdfs = FileSystem.get(conf);
        // hdfs.deleteOnExit(tmpPath);

        if (!wpCleanerJob.waitForCompletion(true)) {
            return 1;
        }

        try {
            Class.forName(driverName);
        } catch (ClassNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            System.exit(1);
        }

        Connection con = DriverManager.getConnection("jdbc:hive://pshdp02:10000", "", "");
        Statement stmt = con.createStatement();
        String sql = "LOAD DATA INPATH '" + tmpPath.toString() + "/part-r-00000' OVERWRITE INTO TABLE LOG_ROWS";
        // String sql = "show tables";
        
        ResultSet result = stmt.executeQuery(sql);

        sql = "INSERT OVERWRITE TABLE CLEAN_LOG_ROWS_EXT SELECT a.* from log_rows a";

        // sql = "SELECT a.* from log_rows a";
        System.out.println("Running: " + sql);
        result = stmt.executeQuery(sql);
        while (result.next()) {
            System.out.println(result.getString(1));
        }

        return 0;
    }

}
