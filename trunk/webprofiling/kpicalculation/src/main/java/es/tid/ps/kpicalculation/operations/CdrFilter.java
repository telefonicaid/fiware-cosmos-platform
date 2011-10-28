package es.tid.ps.kpicalculation.operations;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

import es.tid.ps.kpicalculation.sql.CdrFilterQueries;


public class CdrFilter {

    private Statement stmt;
    
    public CdrFilter(Statement statement) {
        stmt = statement;
    }

    public int process() throws SQLException {
       
      

        ResultSet result = stmt.executeQuery(CdrFilterQueries.CLEAN_BANNED_EXTENSIONS);

        result = stmt.executeQuery(CdrFilterQueries.NORMALISE_LOGS);

        System.out.println("Third party domain URLS erased...");
        result = stmt.executeQuery(CdrFilterQueries.CLEAN_3RD_PARTY_DOMAINS);
        System.out.println("Third party domain URLS erased...");

        System.out.println("Filtering personal info domains...");
        result = stmt.executeQuery(CdrFilterQueries.CLEAN_PERSONAL_DOMAINS);
        System.out.println("Personal info domain erased...");

        System.out.println("Getting page views ...");
        result = stmt.executeQuery(CdrFilterQueries.GENERATE_PAGE_VIEWS);
        System.out.println("Page views generated...");
        
        
        return 0;
    }

}

