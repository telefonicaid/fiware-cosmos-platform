package es.tid.ps.kpicalculation.operations;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
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

    static private Logger logger;

    public AggregateCalculator() {
        logger = Logger.getLogger(AggregateCalculator.class.getName());
        try {
            Connection con = DriverManager.getConnection("jdbc:hive://pshdp02:10000", "", "");
            stmt = con.createStatement();
        } catch (SQLException e) {
            // TODO Auto-generated catch block
            logger.severe(e.toString());
        }

    }

    public int process() {

        // Common aggregate KPI's
        try {

            logger.info("Calculating aggregate of page views by protocol, visitor and device...");
            ResultSet result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_VIS_DEV);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and visitor ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and device...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DEV);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and method...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_METHOD);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROTOCOL);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by protocol ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROTOCOL);
            logger.info("Aggregate processed...");

            // Browing aggregate KPI's

            logger.info("Calculating aggregate of page views by protocol, url and visitor ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_URL_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and url ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_URL);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by protocol and url ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROT_URL);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol, domain and visitor ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DOM_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of page views by protocol and domain ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DOM);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by protocol and domain ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROT_DOM);
            logger.info("Aggregate processed...");

            // Content aggregate KPI's

            logger.info("Calculating aggreagate of page views by category and visitor ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_CAT_VIS);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggreagate of page views by category ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_CAT);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of visitors by category ...");
            result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_CATEGORY);
            logger.info("Aggregate processed...");

            // Queries aggregate KPI's

            logger.info("Calculating aggregate of searches by protocol, query key, visitor and searcher...");
            result = stmt.executeQuery(AggregateCalculatorQueries.SEARCHES_BY_PROT_QKEY_VIS_SEARCHER);
            logger.info("Aggregate processed...");

            logger.info("Calculating aggregate of searches by protocol, query key and searcher...");
            result = stmt.executeQuery(AggregateCalculatorQueries.SEARCHES_BY_PROT_QKEY_VIS);
        } catch (SQLException ex) {
            logger.severe(ex.toString());
        }

        return 0;
    }

}
