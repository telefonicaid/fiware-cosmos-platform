package es.tid.ps.kpicalculation.operations;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import es.tid.ps.kpicalculation.sql.AggregateCalculatorQueries;

public class AggregateCalculator {

    private Statement stmt;
    
    public AggregateCalculator(Statement statement) {
        stmt = statement;
    }

    public int process() throws SQLException {
       
        // Common aggregate KPI's
        
        System.out.println("Calculating aggregate of page views by protocol, visitor and device...");
        ResultSet result = stmt.executeQuery( AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_VIS_DEV );
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of page views by protocol and visitor ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_VIS);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of page views by device...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_DEVICE);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of page views by protocol and method...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_METHOD);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of page views by protocol ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROTOCOL);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of visitors by protocol ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROTOCOL);
        System.out.println("Aggregate processed...");
        
        // Browing aggregate KPI's
        
        System.out.println("Calculating aggregate of page views by protocol, url and visitor ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_URL_VIS);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of page views by protocol and url ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_URL);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of visitors by protocol and url ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROT_URL);
        System.out.println("Aggregate processed...");
        
        System.out.println("Calculating aggregate of page views by protocol, domain and visitor ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DOM_VIS);
        System.out.println("Aggregate processed...");
        
        System.out.println("Calculating aggregate of page views by protocol and domain ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_DOM);
        System.out.println("Aggregate processed...");
        
        System.out.println("Calculating aggregate of visitors by protocol and domain ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_PROT_DOM);
        System.out.println("Aggregate processed...");


        //Content aggregate KPI's
        
        System.out.println("Calculating aggreagate of page views by category and visitor ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_CAT_VIS);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggreagate of page views by category ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.PAGES_VIEWS_BY_PROT_CAT);
        System.out.println("Aggregate processed...");

        System.out.println("Calculating aggregate of visitors by category ...");
        result = stmt.executeQuery(AggregateCalculatorQueries.VISITORS_BY_CATEGORY);
        System.out.println("Aggregate processed...");

        // Queries aggregate KPI's
        
        System.out.println("Calculating aggregate of searches by protocol, query key, visitor and searcher...");
        result = stmt.executeQuery(AggregateCalculatorQueries.SEARCHES_BY_PROT_QKEY_VIS_SEARCHER);
        System.out.println("Aggregate processed...");
        
        System.out.println("Calculating aggregate of searches by protocol, query key and searcher...");
        result = stmt.executeQuery(AggregateCalculatorQueries.SEARCHES_BY_PROT_QKEY_VIS);
        
        
        return 0;
    }

}
