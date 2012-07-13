/* Script to create all the triggers to pass the data from the auxiliary tables to the target tables using and update-insert command */

/* Triggers for page_views */
DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_method_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_method_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_method_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_url_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_url_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_url_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_year_tr;


/* Triggers for visitors */
DROP TRIGGER IF EXISTS psdb.ag_visitors_proto_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_proto_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_proto_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_url_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_url_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_url_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_year_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_day_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_month_tr;
DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_year_tr;
