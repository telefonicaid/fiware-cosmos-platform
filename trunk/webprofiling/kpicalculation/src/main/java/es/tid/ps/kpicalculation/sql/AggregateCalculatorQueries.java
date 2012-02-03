package es.tid.ps.kpicalculation.sql;

public abstract class AggregateCalculatorQueries {
    public static final String PAGES_VIEWS_BY_PROT_VIS_DEV = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_VIS_DEV "
            + "SELECT "
            + "a.visitor_id, a.protocol, a.device, a.date_view, count(*) "
            + "FROM "
            + "page_views a "
            + "GROUP BY "
            + "a.visitor_id, a.protocol, a.device, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_VIS = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_VIS "
            + "SELECT "
            + "a.visitor_id, a.protocol, a.date_view, sum(a.views_num) "
            + "FROM "
            + "ag_page_views_vis_dev a "
            + "GROUP BY "
            + "a.visitor_id, a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_DEV = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_DEVICE "
            + "SELECT "
            + "a.device, a.protocol, a.date_view, sum(a.views_num) "
            + "FROM "
            + "ag_page_views_vis_dev a "
            + "GROUP BY "
            + "a.device, a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_METHOD = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_METHOD "
            + "SELECT "
            + "a.method, a.protocol, a.date_view, count(*) "
            + "FROM "
            + "page_views a "
            + "GROUP BY "
            + "a.method, a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROTOCOL = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS "
            + "SELECT "
            + "a.protocol, a.date_view, sum(a.views_num) "
            + "FROM "
            + "ag_page_views_method a "
            + "GROUP BY "
            + "a.protocol, a.date_view";

    public static final String VISITORS_BY_PROTOCOL = "INSERT OVERWRITE TABLE AG_VISITORS "
            + "SELECT "
            + "a.protocol, a.date_view, count(*) "
            + "FROM "
            + "ag_page_views_vis a " + "GROUP BY " + "a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_URL_VIS = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_URL_VIS "
            + "SELECT "
            + "a.visitor_id, concat(a.url_domain,a.url_path),  a.protocol, a.date_view, count(1) "
            + "FROM "
            + "page_views a "
            + "GROUP BY "
            + "a.visitor_id, concat(a.url_domain,a.url_path) , a.protocol, a.date_view";

    //Duplicated???
    public static final String PAGES_VIEWS_BY_PROT_URL = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_VIS "
            + "SELECT "
            + "a.visitor_id, a.protocol, a.date_view, sum(a.views_num) "
            + "FROM "
            + "ag_page_views_vis_dev a "
            + "GROUP BY "
            + "a.visitor_id, a.protocol, a.date_view";

    public static final String VISITORS_BY_PROT_URL = "INSERT OVERWRITE TABLE AG_VISITORS_URL "
            + "SELECT "
            + "a.page_url, a.protocol, a.date_view, count(*) "
            + "FROM "
            + "ag_page_views_url_vis a "
            + "GROUP BY "
            + "a.page_url, a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_DOM_VIS = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_DOM_VIS "
            + "SELECT "
            + "a.visitor_id, a.url_domain, a.protocol, a.date_view, count(*) "
            + "FROM "
            + "page_views a "
            + "GROUP BY "
            + "a.visitor_id, a.url_domain, a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_DOM = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_DOM "
            + "SELECT "
            + "a.domain, a.protocol, a.date_view, sum(a.views_num) "
            + "FROM "
            + "ag_page_views_dom_vis a "
            + "GROUP BY "
            + "a.domain, a.protocol, a.date_view";

    public static final String VISITORS_BY_PROT_DOM = "INSERT OVERWRITE TABLE AG_VISITORS_DOM "
            + "SELECT "
            + "a.domain, a.protocol, a.date_view, count(*) "
            + "FROM "
            + "ag_page_views_dom_vis a "
            + "GROUP BY "
            + "a.domain, a.protocol, a.date_view";
   

    public static final String PAGES_VIEWS_BY_PROT_CAT_VIS = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_CCAT_VIS "
            + "SELECT "
            + "a.visitor_id, b.catid, a.protocol, a.date_view, count(*) "
            + "FROM "
            + "page_views a "
            + "LEFT OUTER JOIN "
            + "normalised_dmoz b "
            + "ON "
            + "(a.url_domain = parse_url(b.link,'HOST')) "
            + "GROUP BY "
            + "a.visitor_id, b.catid, a.protocol, a.date_view";

    public static final String PAGES_VIEWS_BY_PROT_CAT = "INSERT OVERWRITE TABLE AG_PAGE_VIEWS_CCAT "
            + "SELECT "
            + "a.ccategory, a.protocol, a.date_view, sum(a.views_num) "
            + "FROM "
            + "ag_page_views_ccat_vis a "
            + "GROUP BY "
            + "a.ccategory, a.protocol, a.date_view";

    public static final String VISITORS_BY_CATEGORY = "INSERT OVERWRITE TABLE AG_VISITORS_CCAT "
            + "SELECT "
            + "a.ccategory, a.protocol, a.date_view, count(*) "
            + "FROM "
            + "ag_page_views_ccat_vis a "
            + "GROUP BY "
            + "a.ccategory, a.protocol, a.date_view";

    public static final String SEARCHES_BY_PROT_QKEY_VIS_SEARCHER = "INSERT OVERWRITE TABLE AG_SEARCHES_KEY_VIS_SEA "
            + "SELECT "
            + "a.visitor_id, parse_url(a.full_url,'QUERY',b.key_name), b.name, a.protocol, a.date_view, count(1) "
            + "FROM "
            + "page_views a "
            + "JOIN "
            + "searchers b "
            + "ON "
            + "(concat(a.url_domain,a.url_path) = b.url) "
            + "GROUP BY "
            + "a.visitor_id, parse_url(a.full_url,'QUERY',b.key_name), b.name, a.protocol, a.date_view";

    public static final String SEARCHES_BY_PROT_QKEY_VIS = "INSERT OVERWRITE TABLE AG_SEARCHES_KEY_SEA "
            + "SELECT "
            + "a.key, a.searcher, a.protocol, a.date_view, sum(a.searches_num) "
            + "FROM "
            + "ag_searches_key_vis_sea a "
            + "GROUP BY "
            + "a.key, a.searcher, a.protocol, a.date_view";
}
