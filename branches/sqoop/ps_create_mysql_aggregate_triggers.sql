/* Script to create all the triggers to pass the data from the auxiliary tables to the target tables using and update-insert command */

/* Triggers for page_views */
DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_day_tr;
CREATE TRIGGER psdb.ag_page_views_proto_day_tr AFTER INSERT
    ON psdb.ag_page_views_proto_day_aux FOR EACH ROW insert into psdb.ag_page_views_proto_day (protocol,day,month,year,views_num) values (NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_month_tr;
CREATE TRIGGER psdb.ag_page_views_proto_month_tr AFTER INSERT
    ON psdb.ag_page_views_proto_month_aux FOR EACH ROW insert into psdb.ag_page_views_proto_month (protocol,month,year,views_num) values (NEW.protocol, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_year_tr;
CREATE TRIGGER psdb.ag_page_views_proto_year_tr AFTER INSERT
    ON psdb.ag_page_views_proto_year_aux FOR EACH ROW insert into psdb.ag_page_views_proto_year (protocol,year,views_num) values (NEW.protocol, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_method_day_tr;
CREATE TRIGGER psdb.ag_page_views_method_day_tr AFTER INSERT
    ON psdb.ag_page_views_method_day_aux FOR EACH ROW insert into psdb.ag_page_views_method_day (method,day,month,year,views_num) values (NEW.method, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_method_month_tr;
CREATE TRIGGER psdb.ag_page_views_method_month_tr AFTER INSERT
    ON psdb.ag_page_views_method_month_aux FOR EACH ROW insert into psdb.ag_page_views_method_month (method,month,year,views_num) values (NEW.method, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_method_year_tr;
CREATE TRIGGER psdb.ag_page_views_method_year_tr AFTER INSERT
    ON psdb.ag_page_views_method_year_aux FOR EACH ROW insert into psdb.ag_page_views_method_year (method,year,views_num) values (NEW.method, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_day_tr;
CREATE TRIGGER psdb.ag_page_views_uagent_day_tr AFTER INSERT
    ON psdb.ag_page_views_uagent_day_aux FOR EACH ROW insert into psdb.ag_page_views_uagent_day (user_agent,day,month,year,views_num) values (NEW.user_agent, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_month_tr;
CREATE TRIGGER psdb.ag_page_views_uagent_month_tr AFTER INSERT
    ON psdb.ag_page_views_uagent_month_aux FOR EACH ROW insert into psdb.ag_page_views_uagent_month (user_agent,month,year,views_num) values (NEW.user_agent, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_year_tr;
CREATE TRIGGER psdb.ag_page_views_uagent_year_tr AFTER INSERT
    ON psdb.ag_page_views_uagent_year_aux FOR EACH ROW insert into psdb.ag_page_views_uagent_year (user_agent,year,views_num) values (NEW.user_agent, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_url_day_tr;
CREATE TRIGGER psdb.ag_page_views_url_day_tr AFTER INSERT
    ON psdb.ag_page_views_url_day_aux FOR EACH ROW insert into psdb.ag_page_views_url_day (url,day,month,year,views_num) values (NEW.url, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_url_month_tr;
CREATE TRIGGER psdb.ag_page_views_url_month_tr AFTER INSERT
    ON psdb.ag_page_views_url_month_aux FOR EACH ROW insert into psdb.ag_page_views_url_month (url,month,year,views_num) values (NEW.url, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_url_year_tr;
CREATE TRIGGER psdb.ag_page_views_url_year_tr AFTER INSERT
    ON psdb.ag_page_views_url_year_aux FOR EACH ROW insert into psdb.ag_page_views_url_year (url,year,views_num) values (NEW.url, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_day_tr;
CREATE TRIGGER psdb.ag_page_views_dom_day_tr AFTER INSERT
    ON psdb.ag_page_views_dom_day_aux FOR EACH ROW insert into psdb.ag_page_views_dom_day (domain,day,month,year,views_num) values (NEW.domain, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_month_tr;
CREATE TRIGGER psdb.ag_page_views_dom_month_tr AFTER INSERT
    ON psdb.ag_page_views_dom_month_aux FOR EACH ROW insert into psdb.ag_page_views_dom_month (domain,month,year,views_num) values (NEW.domain, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_year_tr;
CREATE TRIGGER psdb.ag_page_views_dom_year_tr AFTER INSERT
    ON psdb.ag_page_views_dom_year_aux FOR EACH ROW insert into psdb.ag_page_views_dom_year (domain,year,views_num) values (NEW.domain, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_day_tr;
CREATE TRIGGER psdb.ag_page_views_ccat_day_tr AFTER INSERT
    ON psdb.ag_page_views_ccat_day_aux FOR EACH ROW insert into psdb.ag_page_views_ccat_day (content_category,day,month,year,views_num) values (NEW.content_category, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_month_tr;
CREATE TRIGGER psdb.ag_page_views_ccat_month_tr AFTER INSERT
    ON psdb.ag_page_views_ccat_month_aux FOR EACH ROW insert into psdb.ag_page_views_ccat_month (content_category,month,year,views_num) values (NEW.content_category, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_year_tr;
CREATE TRIGGER psdb.ag_page_views_ccat_year_tr AFTER INSERT
    ON psdb.ag_page_views_ccat_year_aux FOR EACH ROW insert into psdb.ag_page_views_ccat_year (content_category,year,views_num) values (NEW.content_category, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num;

DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_day_tr;
CREATE TRIGGER psdb.ag_searches_key_sea_day_tr AFTER INSERT
    ON psdb.ag_searches_key_sea_day_aux FOR EACH ROW insert into psdb.ag_searches_key_sea_day (key_sea,searcher,day,month,year,searches_num) values (NEW.key_sea, NEW.searcher, NEW.day, NEW.month, NEW.year, NEW.searches_num) ON DUPLICATE KEY UPDATE searches_num = NEW.searches_num;

DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_month_tr;
CREATE TRIGGER psdb.ag_searches_key_sea_month_tr AFTER INSERT
    ON psdb.ag_searches_key_sea_month_aux FOR EACH ROW insert into psdb.ag_searches_key_sea_month (key_sea,searcher,month,year,searches_num) values (NEW.key_sea, NEW.searcher, NEW.month, NEW.year, NEW.searches_num) ON DUPLICATE KEY UPDATE searches_num = NEW.searches_num;

DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_year_tr;
CREATE TRIGGER psdb.ag_searches_key_sea_year_tr AFTER INSERT
    ON psdb.ag_searches_key_sea_year_aux FOR EACH ROW insert into psdb.ag_searches_key_sea_year (key_sea,searcher,year,searches_num) values (NEW.key_sea, NEW.searcher, NEW.year, NEW.searches_num) ON DUPLICATE KEY UPDATE searches_num = NEW.searches_num;


/* Triggers for visitors */
DROP TRIGGER IF EXISTS psdb.ag_visitors_proto_day_tr;
CREATE TRIGGER psdb.ag_visitors_proto_day_tr AFTER INSERT
    ON psdb.ag_visitors_proto_day_aux FOR EACH ROW insert into psdb.ag_visitors_proto_day (protocol,day,month,year,visitors_num) values (NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_proto_month_tr;
CREATE TRIGGER psdb.ag_visitors_proto_month_tr AFTER INSERT
    ON psdb.ag_visitors_proto_month_aux FOR EACH ROW insert into psdb.ag_visitors_proto_month (protocol,month,year,visitors_num) values (NEW.protocol, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_proto_year_tr;
CREATE TRIGGER psdb.ag_visitors_proto_year_tr AFTER INSERT
    ON psdb.ag_visitors_proto_year_aux FOR EACH ROW insert into psdb.ag_visitors_proto_year (protocol,year,visitors_num) values (NEW.protocol, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_day_tr;
CREATE TRIGGER psdb.ag_visitors_uagent_day_tr AFTER INSERT
    ON psdb.ag_visitors_uagent_day_aux FOR EACH ROW insert into psdb.ag_visitors_uagent_day (user_agent,day,month,year,visitors_num) values (NEW.user_agent, NEW.day, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_month_tr;
CREATE TRIGGER psdb.ag_visitors_uagent_month_tr AFTER INSERT
    ON psdb.ag_visitors_uagent_month_aux FOR EACH ROW insert into psdb.ag_visitors_uagent_month (user_agent,month,year,visitors_num) values (NEW.user_agent, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_year_tr;
CREATE TRIGGER psdb.ag_visitors_uagent_year_tr AFTER INSERT
    ON psdb.ag_visitors_uagent_year_aux FOR EACH ROW insert into psdb.ag_visitors_uagent_year (user_agent,year,visitors_num) values (NEW.user_agent, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;


DROP TRIGGER IF EXISTS psdb.ag_visitors_url_day_tr;
CREATE TRIGGER psdb.ag_visitors_url_day_tr AFTER INSERT
    ON psdb.ag_visitors_url_day_aux FOR EACH ROW insert into psdb.ag_visitors_url_day (url,day,month,year,visitors_num) values (NEW.url, NEW.day, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_url_month_tr;
CREATE TRIGGER psdb.ag_visitors_url_month_tr AFTER INSERT
    ON psdb.ag_visitors_url_month_aux FOR EACH ROW insert into psdb.ag_visitors_url_month (url,month,year,visitors_num) values (NEW.url, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_url_year_tr;
CREATE TRIGGER psdb.ag_visitors_url_year_tr AFTER INSERT
    ON psdb.ag_visitors_url_year_aux FOR EACH ROW insert into psdb.ag_visitors_url_year (url,year,visitors_num) values (NEW.url, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;


DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_day_tr;
CREATE TRIGGER psdb.ag_visitors_dom_day_tr AFTER INSERT
    ON psdb.ag_visitors_dom_day_aux FOR EACH ROW insert into psdb.ag_visitors_dom_day (domain,day,month,year,visitors_num) values (NEW.domain, NEW.day, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_month_tr;
CREATE TRIGGER psdb.ag_visitors_dom_month_tr AFTER INSERT
    ON psdb.ag_visitors_dom_month_aux FOR EACH ROW insert into psdb.ag_visitors_dom_month (domain,month,year,visitors_num) values (NEW.domain, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_year_tr;
CREATE TRIGGER psdb.ag_visitors_dom_year_tr AFTER INSERT
    ON psdb.ag_visitors_dom_year_aux FOR EACH ROW insert into psdb.ag_visitors_dom_year (domain,year,visitors_num) values (NEW.domain, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;


DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_day_tr;
CREATE TRIGGER psdb.ag_visitors_ccat_day_tr AFTER INSERT
    ON psdb.ag_visitors_ccat_day_aux FOR EACH ROW insert into psdb.ag_visitors_ccat_day (content_category,day,month,year,visitors_num) values (NEW.content_category, NEW.day, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_month_tr;
CREATE TRIGGER psdb.ag_visitors_ccat_month_tr AFTER INSERT
    ON psdb.ag_visitors_ccat_month_aux FOR EACH ROW insert into psdb.ag_visitors_ccat_month (content_category,month,year,visitors_num) values (NEW.content_category, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_year_tr;
CREATE TRIGGER psdb.ag_visitors_ccat_year_tr AFTER INSERT
    ON psdb.ag_visitors_ccat_year_aux FOR EACH ROW insert into psdb.ag_visitors_ccat_year (content_category,year,visitors_num) values (NEW.content_category, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;