package es.tid.ps.kpicalculation.sql;

public class CdrFilterQueries {

    final static public String CLEAN_BANNED_EXTENSIONS = 
            "INSERT OVERWRITE TABLE CLEAN_LOG_ROWS_EXT " +
                "SELECT " +
                    "a.* " +
                "FROM " +
                    "log_rows a " +
                "LEFT OUTER JOIN " +
                    "banned_extensions b " +
                "ON " +
                    "(lower(regexp_extract(parse_url(a.url,'PATH'),'^\\/(.*)([\\.]+)([a-zA-Z0-9]+)',3)) = b.extension_id) " +
                "WHERE " +
                    "b.extension_id is NULL";
    final static public String NORMALISE_LOGS = 
            "INSERT OVERWRITE TABLE NORMALISED_LOG_ROWS " +
                "select " +
                    "a.user_id, concat(parse_url(a.url,'PROTOCOL'),'://',if(b.extension is null,regexp_extract(parse_url" +
                    "(a.url,'HOST'),'([0-9a-zA-Z\\-_]*\\.[0-9a-zA-Z]*)$',1),regexp_extract(parse_url(a.url,'HOST'),'" +
                    "([0-9a-zA-Z\\-_]*\\.[0-9a-zA-Z]*\\.[0-9a-zA-Z]*)$',1)),parse_url(a.url,'FILE')),a.date_time, " +
                    "a.http_status, a.mime_type, a.user_agent, a.method " +
                "from " +
                    "clean_log_rows_ext a " +
                "left outer join " +
                    "normalised_ext b " +
                "on " +
                    "(regexp_extract(parse_url(a.url,'HOST'),'([a-zA-Z0-9]*)\\.([a-zA-Z0-9]*)$',1) = b.extension)";
    final static public String CLEAN_3RD_PARTY_DOMAINS = 
            "INSERT OVERWRITE TABLE PREP_LOG_ROWS_TPDOM " +
                "SELECT " +
                    "a.user_id, a.url, parse_url(a.url, 'PROTOCOL'), parse_url(a.url, 'HOST'), parse_url(a.url, 'PATH'), " +
                    "parse_url(a.url, 'QUERY'), a.date_time, a.http_status, a.mime_type, a.user_agent, a.method " +
                "from " +
                    "normalised_log_rows a " +
                "left outer join " +
                    "third_party_domains b " +
                "ON " +
                    "(parse_url(a.url,'HOST') = b.domain_id) where b.domain_id is null";
    
    final static public String CLEAN_PERSONAL_DOMAINS = 
            "INSERT OVERWRITE TABLE PREP_LOG_ROWS_PIDOM " +
                "SELECT " +
                    "a.* " +
                "from " +
                    "prep_log_rows_tpdom a " +
                "left outer join " +
                    "personal_info_domains b " +
                "ON " +
                    "(a.url_domain = b.domain_id) where b.domain_id is null";
    
    final static public String GENERATE_PAGE_VIEWS = 
            "INSERT OVERWRITE TABLE PAGE_VIEWS " +
                "select " +
                    "user_id, protocol, full_url, url_domain, url_path, url_query, regexp_extract(date_time,'([0-9]{1,2}[a-zA-Z]" +
                    "{3}[0-9]{4})',1) ,regexp_extract(date_time,'([0-9]{6})$',1) , user_agent, user_agent, user_agent, user_agent, " +
                    "method, http_status " +
                "from " +
                    "PREP_LOG_ROWS_PIDOM";
}
