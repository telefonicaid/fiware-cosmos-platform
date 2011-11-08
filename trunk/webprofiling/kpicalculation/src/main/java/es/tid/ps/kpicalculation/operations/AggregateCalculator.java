package es.tid.ps.kpicalculation.operations;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.logging.Logger;

import es.tid.ps.kpicalculation.sql.AggregateCalculatorQueries;

/**
 * This class calculates aggregate statistics for webprofling module of PS using
 * data stored in a hive database. For calculating this values, a set of SQL
 * queries contained in the AggregateCalculatorQueries class over hive are
 * executed. This queries sensitive to the order they are executed because some
 * of them can be dependent on the previous ones.
 * 
 * TODO:Make configurable the aggregates that are calculated.
 * 
 * @author javierb
 * 
 */
public class AggregateCalculator implements IAggregateCalculator {
    private Statement stmt;

    private static String driverName = "org.apache.hadoop.hive.jdbc.HiveDriver";
    private static Logger logger;

    /**
     * Constructor method. Its only aim is to initialize the connection to hive
     * database where aggregates will be calculated
     * 
     */
    public AggregateCalculator() {
        logger = Logger.getLogger(AggregateCalculator.class.getName());
        try {
            Class.forName(driverName);
            Connection con = DriverManager.getConnection(
                    "jdbc:hive://pshdp02:10000", "", "");
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
     * Performs all the queries to calculate the aggregates of web profiling
     * module.
     * 
     */
    public int process() {
        // Common aggregate KPI's
        try {
            logger.info("Calculating aggregate of page views by protocol, visitor and device...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_VIS_DEV);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and visitor ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and device...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DEV);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and method...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_METHOD);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROTOCOL);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by protocol ...");
            stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROTOCOL);
            logger.info("Aggregate processed...");

            // Browing aggregate KPI's
            logger.info("Calculating aggregate of page views by protocol, url and visitor ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_URL_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and url ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_URL);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by protocol and url ...");
            stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROT_URL);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol, domain and visitor ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DOM_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and domain ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DOM);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by protocol and domain ...");
            stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROT_DOM);
            logger.info("Aggregate processed...");

            // Content aggregate KPI's

            logger.info("Calculating aggreagate of page views by category and visitor ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_CAT_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggreagate of page views by category ...");
            stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_CAT);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by category ...");
            stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_CATEGORY);
            logger.info("Aggregate processed...");

            // Queries aggregate KPI's
            logger.info("Calculating aggregate of searches by protocol, query key, visitor and searcher...");
            stmt.executeQuery(AggregateCalculatorQueries.SEARCHES_BY_PROT_QKEY_VIS_SEARCHER);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of searches by protocol, query key and searcher...");
            stmt.executeQuery(AggregateCalculatorQueries.SEARCHES_BY_PROT_QKEY_VIS);
        } catch (SQLException ex) {
            logger.severe(ex.toString());
        }
        return 0;
    }

}
