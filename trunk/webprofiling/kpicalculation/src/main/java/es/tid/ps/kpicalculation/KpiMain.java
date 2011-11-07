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

import es.tid.ps.kpicalculation.operations.AggregateCalculator;
import es.tid.ps.kpicalculation.operations.CdrFilter;
import es.tid.ps.kpicalculation.operations.IAggregateCalculator;

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

        if (!wpCleanerJob.waitForCompletion(true)) {
            return 1;
        }

       
       // Load data into table LOG_ROWS
       // TODO(javierb): At this moment only one file is loaded. It will be
       // necessary to
       // change it to take a folder as input
       
       /* String sql = "LOAD DATA INPATH '" + tmpPath.toString() + "/part-r-00000' OVERWRITE INTO TABLE LOG_ROWS";
        ResultSet result = stmt.executeQuery(sql);
        */
        
        IAggregateCalculator agg = new AggregateCalculator();
        agg.process();
        

        return 0;
    }
}