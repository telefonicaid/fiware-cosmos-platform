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

        // Establish connection with hive database.
        // TODO(javierb): Try to implement direct connection without using
        // hiveserver
        Connection con = DriverManager.getConnection("jdbc:hive://pshdp02:10000", "", "");

        // Load data into table LOG_ROWS
        // TODO(javierb): At this moment only one file is loaded. It will be
        // necessary to
        // change it to take a folder as input
        Statement stmt = con.createStatement();
        String sql = "LOAD DATA INPATH '" + tmpPath.toString() + "/part-r-00000' OVERWRITE INTO TABLE LOG_ROWS";
        ResultSet result = stmt.executeQuery(sql);
        // sql =
        // "INSERT OVERWRITE TABLE CLEAN_LOG_ROWS_EXT SELECT a.* from log_rows a";
        // result = stmt.executeQuery(sql);

        // "run-process-wp.sh" BEGIN
        // "run-clean-log-rows-ext.sh" BEGIN
        sql = "INSERT OVERWRITE TABLE CLEAN_LOG_ROWS_EXT SELECT a.* from o2_profile a left outer join banned_extensions b ON (lower(regexp_extract(parse_url(a.url,'PATH'),'^\\/(.*)([\\.]+)([a-zA-Z0-9]+)',3)) = b.extension_id) WHERE b.extension_id is NULL";
        result = stmt.executeQuery(sql);
        // "run-clean-log-rows-ext.sh" END

        // "run-normalised-log-rows.sh" BEGIN
        sql = "INSERT OVERWRITE TABLE NORMALISED_LOG_ROWS select a.user_id, concat(parse_url(a.url,'PROTOCOL'),'://',if(b.extension is null,regexp_extract(parse_url(a.url,'HOST'),'([0-9a-zA-Z\\-_]*\\.[0-9a-zA-Z]*)$',1),regexp_extract(parse_url(a.url,'HOST'),'([0-9a-zA-Z\\-_]*\\.[0-9a-zA-Z]*\\.[0-9a-zA-Z]*)$',1)),parse_url(a.url,'FILE')),a.date_time, a.http_status, a.mime_type, a.user_agent, a.method from clean_log_rows_ext a left outer join normalised_ext b on (regexp_extract(parse_url(a.url,'HOST'),'([a-zA-Z0-9]*)\\.([a-zA-Z0-9]*)$',1) = b.extension)";
        result = stmt.executeQuery(sql);
        // "run-normalised-log-rows.sh" END

        // "run-prep-log-rows-tpdom" BEGIN
        System.out.println("Third party domain URLS erased...");
        sql = "INSERT OVERWRITE TABLE PREP_LOG_ROWS_TPDOM SELECT a.user_id, a.url, parse_url(a.url, 'PROTOCOL'), parse_url(a.url, 'HOST'), parse_url(a.url, 'PATH'), parse_url(a.url, 'QUERY'), a.date_time, a.http_status, a.mime_type, a.user_agent, a.method from normalised_log_rows a left outer join third_party_domains b ON (parse_url(a.url,'HOST') = b.domain_id) where b.domain_id is null";
        result = stmt.executeQuery(sql);
        System.out.println("Third party domain URLS erased...");
        // "run-prep-log-rows-tpdom" END

        // "run-prep-log-rows-pidom.sh" BEGIN
        System.out.println("Filtering personal info domains...");
        sql = "INSERT OVERWRITE TABLE PREP_LOG_ROWS_PIDOM SELECT a.* from prep_log_rows_tpdom a left outer join personal_info_domains b ON (a.url_domain = b.domain_id) where b.domain_id is null";
        result = stmt.executeQuery(sql);
        System.out.println("Personal info domain erased...");
        // "run-prep-log-rows-pidom.sh" END

        // "run-proc-page-views.sh" BEGIN
        System.out.println("Getting page views ...");
        sql = "INSERT OVERWRITE TABLE PAGE_VIEWS select user_id, protocol, full_url, url_domain, url_path, url_query, regexp_extract(date_time,'([0-9]{1,2}[a-zA-Z]{3}[0-9]{4})',1) ,regexp_extract(date_time,'([0-9]{6})$',1) , user_agent, user_agent, user_agent, user_agent, method, http_status from PREP_LOG_ROWS_PIDOM";
        result = stmt.executeQuery(sql);
        System.out.println("Page views generated...");
        // "run-proc-page-views.sh" END
        // "run-process-wp.sh" END

        // "run-aggregates-wp.sh" BEGIN
        // "run-aggregates-kpi.sh" BEGIN
        // "run-ag-page-views-vis-dev.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol, visitor and device...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_VIS_DEV select a.visitor_id, a.protocol, a.device, a.date_view, count(*) from page_views a group by a.visitor_id, a.protocol, a.device, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-vis-dev.sh" END
        // "run-ag-page-views-vis.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol and visitor ...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_VIS select a.visitor_id, a.protocol, a.date_view, sum(a.views_num) from ag_page_views_vis_dev a group by a.visitor_id, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-vis.sh" END
        // "run-ag-page-views-device.sh" BEGIN
        System.out.println("Calculating aggregate of page views by device...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_DEVICE select a.device, a.protocol, a.date_view, sum(a.views_num) from ag_page_views_vis_dev a group by a.device, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-device.sh" END
        // "run-ag-page-views-method.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol and method...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_METHOD select a.method, a.protocol, a.date_view, count(*) from page_views a group by a.method, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-method.sh" END
        // "run-ag-page-views.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol ...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS select a.protocol, a.date_view, sum(a.views_num) from ag_page_views_method a group by a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views.sh" END
        // "run-ag-visitors.sh" BEGIN
        System.out.println("Calculating aggregate of visitors by protocol ...");
        sql = "INSERT OVERWRITE TABLE AG_VISITORS select a.protocol, a.date_view, count(*) from ag_page_views_vis a group by a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-visitors.sh" END
        // "run-aggregates-kpi.sh" END
        // "run-aggregates-browsing.sh" BEGIN
        // "run-ag-page-views-url-vis.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol, url and visitor ...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_URL_VIS select a.visitor_id, concat(a.url_domain,a.url_path),  a.protocol, a.date_view, count(1) from page_views a group by a.visitor_id, concat(a.url_domain,a.url_path) , a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-url-vis.sh" END
        // "run-ag-page-views-url.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol and url ...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_VIS select a.visitor_id, a.protocol, a.date_view, sum(a.views_num) from ag_page_views_vis_dev a group by a.visitor_id, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-url.sh" END
        // "run-ag-visitors-url.sh" BEGIN
        System.out.println("Calculating aggregate of visitors by protocol and url ...");
        sql = "INSERT OVERWRITE TABLE AG_VISITORS_URL select a.page_url, a.protocol, a.date_view, count(*) from ag_page_views_url_vis a group by a.page_url, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-visitors-url.sh" END
        // "run-ag-page-views-dom-vis.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol, domain and visitor ...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_DOM_VIS select a.visitor_id, a.url_domain, a.protocol, a.date_view, count(*) from page_views a group by a.visitor_id, a.url_domain, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-dom-vis.sh" END
        // "run-ag-page-views-dom.sh" BEGIN
        System.out.println("Calculating aggregate of page views by protocol and domain ...");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_DOM select a.domain, a.protocol, a.date_view, sum(a.views_num) from ag_page_views_dom_vis a group by a.domain, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-dom.sh" END
        // "run-ag-visitors-dom.sh" BEGIN
        System.out.println("Calculating aggregate of visitors by protocol and domain ...");
        sql = "INSERT OVERWRITE TABLE AG_VISITORS_DOM select a.domain, a.protocol, a.date_view, count(*) from ag_page_views_dom_vis a group by a.domain, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-visitors-dom.sh" END
        // "run-aggregates-browsing.sh" END
        // "run-aggregates-content.sh" BEGIN
        // "run-ag-page-views-ccat-vis.sh" BEGIN
        System.out.println("Unkonwn meaning of this aggreagate");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_CCAT_VIS select a.visitor_id, b.catid, a.protocol, a.date_view, count(*) from page_views a left outer join normalised_dmoz b ON (a.url_domain = parse_url(b.link,'HOST')) group by a.visitor_id, b.catid, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-ccat-vis.sh" END
        // "run-ag-page-views-ccat.sh" BEGIN
        System.out.println("Unkonwn meaning of this aggreagate");
        sql = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_CCAT select a.ccategory, a.protocol, a.date_view, sum(a.views_num) from ag_page_views_ccat_vis a group by a.ccategory, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-page-views-ccat.sh" END
        // "run-ag-visitors-ccat.sh" BEGIN
        System.out.println("Unkonwn meaning of this aggreagate");
        sql = "INSERT OVERWRITE TABLE AG_VISITORS_CCAT select a.ccategory, a.protocol, a.date_view, count(*) from ag_page_views_ccat_vis a group by a.ccategory, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-visitors-ccat.sh" END
        // "run-aggregates-content.sh" END
        // "run-aggregates-queries.sh" BEGIN
        // "run-ag-searches-key-vis-sea.sh" BEGIN
        System.out.println("Calculating aggregate of searches by protocol, query key, visitor and searcher...");
        sql = "INSERT OVERWRITE TABLE AG_SEARCHES_KEY_VIS_SEA select a.visitor_id, parse_url(a.full_url,'QUERY',b.key_name), b.name, a.protocol, a.date_view, count(1) from page_views a JOIN searchers b ON (concat(a.url_domain,a.url_path) = b.url) group by a.visitor_id, parse_url(a.full_url,'QUERY',b.key_name), b.name, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-searches-key-vis-sea.sh" END
        // "run-ag-searches-key-sea.sh" BEGIN
        System.out.println("Calculating aggregate of searches by protocol, query key and searcher...");
        sql = "INSERT OVERWRITE TABLE AG_SEARCHES_KEY_SEA select a.key, a.searcher, a.protocol, a.date_view, sum(a.searches_num) from ag_searches_key_vis_sea a group by a.key, a.searcher, a.protocol, a.date_view";
        result = stmt.executeQuery(sql);
        System.out.println("Aggregate processed...");
        // "run-ag-searches-key-sea.sh" END
        // "run-aggregates-queries.sh" END
        // "run-aggregates-wp.sh" END

        return 0;
    }
}
