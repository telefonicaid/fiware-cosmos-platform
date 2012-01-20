/* Script to create all the triggers to pass the data from the auxiliary tables to the target tables using and update-insert command */

/* Triggers for page_views */
DROP TRIGGER IF EXISTS psdb.ag_page_views_proto_day_tr;
CREATE TRIGGER psdb.ag_page_views_proto_day_tr AFTER INSERT
    ON psdb.ag_page_views_proto_day_aux FOR EACH ROW insert into psdb.ag_page_views_proto_day (protocol,day,month,year,views_num) values (NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num + views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_method_day_tr;
CREATE TRIGGER psdb.ag_page_views_method_day_tr AFTER INSERT
    ON psdb.ag_page_views_method_day_aux FOR EACH ROW insert into psdb.ag_page_views_method_day (method,protocol,day,month,year,views_num) values (NEW.method, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num + views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_uagent_day_tr;
CREATE TRIGGER psdb.ag_page_views_uagent_day_tr AFTER INSERT
    ON psdb.ag_page_views_uagent_day_aux FOR EACH ROW insert into psdb.ag_page_views_uagent_day (user_agent_id,protocol,day,month,year,user_agent_desc,views_num) values (NEW.user_agent_id, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.user_agent_desc, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num + views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_dom_path_day_tr;
CREATE TRIGGER psdb.ag_page_views_dom_path_day_tr AFTER INSERT
    ON psdb.ag_page_views_dom_path_day_aux FOR EACH ROW insert into psdb.ag_page_views_dom_path_day (url_id,protocol,day,month,year,dom_desc,path_desc,views_num) values (NEW.url_id, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.dom_desc, NEW.path_desc, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num + views_num;

DROP TRIGGER IF EXISTS psdb.ag_page_views_ccat_day_tr;
CREATE TRIGGER psdb.ag_page_views_ccat_day_tr AFTER INSERT
    ON psdb.ag_page_views_ccat_day_aux FOR EACH ROW insert into psdb.ag_page_views_ccat_day (content_category_id,protocol,day,month,year,views_num) values (NEW.content_category_id, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.views_num) ON DUPLICATE KEY UPDATE views_num = NEW.views_num + views_num;

DROP TRIGGER IF EXISTS psdb.ag_searches_key_sea_day_tr;
CREATE TRIGGER psdb.ag_searches_key_sea_day_tr AFTER INSERT
    ON psdb.ag_searches_key_sea_day_aux FOR EACH ROW insert into psdb.ag_searches_key_sea_day (key_sea,searcher,protocol,day,month,year,searches_num) values (NEW.key_sea, NEW.searcher, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.searches_num) ON DUPLICATE KEY UPDATE searches_num = NEW.searches_num + searches_num;


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
    ON psdb.ag_visitors_uagent_day_aux FOR EACH ROW insert into psdb.ag_visitors_uagent_day (user_agent_id,protocol,day,month,year,user_agent_desc,visitors_num) values (NEW.user_agent_id, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.user_agent_desc, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_month_tr;
CREATE TRIGGER psdb.ag_visitors_uagent_month_tr AFTER INSERT
    ON psdb.ag_visitors_uagent_month_aux FOR EACH ROW insert into psdb.ag_visitors_uagent_month (user_agent_id,protocol,month,year,user_agent_desc,visitors_num) values (NEW.user_agent_id, NEW.protocol, NEW.month, NEW.year, NEW.user_agent_desc, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_uagent_year_tr;
CREATE TRIGGER psdb.ag_visitors_uagent_year_tr AFTER INSERT
    ON psdb.ag_visitors_uagent_year_aux FOR EACH ROW insert into psdb.ag_visitors_uagent_year (user_agent_id,protocol,year,user_agent_desc,visitors_num) values (NEW.user_agent_id, NEW.protocol, NEW.year, NEW.user_agent_desc, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_path_day_tr;
CREATE TRIGGER psdb.ag_visitors_dom_path_day_tr AFTER INSERT
    ON psdb.ag_visitors_dom_path_day_aux FOR EACH ROW insert into psdb.ag_visitors_dom_path_day (url_id,protocol,day,month,year,dom_desc,path_desc,visitors_num) values (NEW.url_id, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.dom_desc, NEW.path_desc, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_path_month_tr;
CREATE TRIGGER psdb.ag_visitors_dom_path_month_tr AFTER INSERT
    ON psdb.ag_visitors_dom_path_month_aux FOR EACH ROW insert into psdb.ag_visitors_dom_path_month (url_id,protocol,month,year,dom_desc,path_desc,visitors_num) values (NEW.url_id, NEW.protocol, NEW.month, NEW.year, NEW.dom_desc, NEW.path_desc, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_dom_path_year_tr;
CREATE TRIGGER psdb.ag_visitors_dom_path_year_tr AFTER INSERT
    ON psdb.ag_visitors_dom_path_year_aux FOR EACH ROW insert into psdb.ag_visitors_dom_path_year (url_id,protocol,year,dom_desc,path_desc,visitors_num) values (NEW.url_id, NEW.protocol, NEW.year, NEW.dom_desc, NEW.path_desc, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_day_tr;
CREATE TRIGGER psdb.ag_visitors_ccat_day_tr AFTER INSERT
    ON psdb.ag_visitors_ccat_day_aux FOR EACH ROW insert into psdb.ag_visitors_ccat_day (content_category,protocol,day,month,year,visitors_num) values (NEW.content_category, NEW.protocol, NEW.day, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_month_tr;
CREATE TRIGGER psdb.ag_visitors_ccat_month_tr AFTER INSERT
    ON psdb.ag_visitors_ccat_month_aux FOR EACH ROW insert into psdb.ag_visitors_ccat_month (content_category,protocol,month,year,visitors_num) values (NEW.content_category, NEW.protocol, NEW.month, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num;

DROP TRIGGER IF EXISTS psdb.ag_visitors_ccat_year_tr;
CREATE TRIGGER psdb.ag_visitors_ccat_year_tr AFTER INSERT
    ON psdb.ag_visitors_ccat_year_aux FOR EACH ROW insert into psdb.ag_visitors_ccat_year (content_category,protocol,year,visitors_num) values (NEW.content_category, NEW.protocol, NEW.year, NEW.visitors_num) ON DUPLICATE KEY UPDATE visitors_num = NEW.visitors_num + visitors_num;