/* Script to create all target and auxiliary tables in the mysql database psdb where the export data will be loaded */

/* First check if the database exists */
DROP DATABASE IF EXISTS psdb;
CREATE DATABASE psdb;

/* Create the target table in the usually way. In the next command the auxiliary tables are created like a copy of the target ones */ 
/* Page views tables */
DROP TABLE IF EXISTS psdb.ag_page_views_proto_day;
CREATE TABLE  psdb.ag_page_views_proto_day (
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL
  PRIMARY KEY (protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_proto_day_aux;
CREATE TABLE  psdb.ag_page_views_proto_day_aux (
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_proto_month;
CREATE TABLE  psdb.ag_page_views_proto_month (
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_proto_year;
CREATE TABLE  psdb.ag_page_views_proto_year (
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_method_day;
CREATE TABLE  psdb.ag_page_views_method_day (
  method varchar(20) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (method,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_method_day_aux;
CREATE TABLE  psdb.ag_page_views_method_day_aux (
  method varchar(20) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_method_month;
CREATE TABLE  psdb.ag_page_views_method_month (
  method varchar(20) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (method,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_method_year;
CREATE TABLE  psdb.ag_page_views_method_year (
  method varchar(20) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (method,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_uagent_day;
CREATE TABLE  psdb.ag_page_views_uagent_day (
  user_agent_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent_id,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_day_aux;
CREATE TABLE  psdb.ag_page_views_uagent_day_aux (
  user_agent_id int(255) NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_month;
CREATE TABLE  psdb.ag_page_views_uagent_month (
  user_agent_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent_id,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_year;
CREATE TABLE  psdb.ag_page_views_uagent_year (
  user_agent_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent_id,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_dom_path_day;
CREATE TABLE  psdb.ag_page_views_dom_path_day (
  url_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url_id,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_path_day_aux;
CREATE TABLE  psdb.ag_page_views_dom_path_day_aux (
  url_id int(255) NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_path_month;
CREATE TABLE  psdb.ag_page_views_dom_path_month (
  url_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url_id,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_path_year;
CREATE TABLE  psdb.ag_page_views_dom_path_year (
  url_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url_id,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_day;
CREATE TABLE  psdb.ag_page_views_ccat_day (
  content_category_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category_id,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_day_aux;
CREATE TABLE  psdb.ag_page_views_ccat_day_aux (
  content_category_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_month;
CREATE TABLE  psdb.ag_page_views_ccat_month (
  content_category_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category_id,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_page_views_ccat_year;
CREATE TABLE  psdb.ag_page_views_ccat_year (
  content_category_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category_id,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_searches_key_sea_day;
CREATE TABLE  psdb.ag_searches_key_sea_day (
  key_sea varchar(275) NOT NULL,
  searcher varchar(40) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL,
  PRIMARY KEY (key_sea,searcher,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_searches_key_sea_day_aux;
CREATE TABLE  psdb.ag_searches_key_sea_day_aux (
  key_sea varchar(1000) NOT NULL,
  searcher varchar(200) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_searches_key_sea_month;
CREATE TABLE  psdb.ag_searches_key_sea_month (
  key_sea varchar(275) NOT NULL,
  searcher varchar(40) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL,
  PRIMARY KEY (key_sea,searcher,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_searches_key_sea_year;
CREATE TABLE  psdb.ag_searches_key_sea_year (
  key_sea varchar(275) NOT NULL,
  searcher varchar(40) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL,
  PRIMARY KEY (key_sea,searcher,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


/* Visitors tables */
DROP TABLE IF EXISTS psdb.ag_visitors_proto_day;
CREATE TABLE psdb.ag_visitors_proto_day (
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_day_aux;
CREATE TABLE psdb.ag_visitors_proto_day_aux (
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_visitors_proto_month;
CREATE TABLE  psdb.ag_visitors_proto_month (
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_month_aux;
CREATE TABLE  psdb.ag_visitors_proto_month_aux (
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_year;
CREATE TABLE  psdb.ag_visitors_proto_year (
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_year_aux;
CREATE TABLE  psdb.ag_visitors_proto_year_aux (
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_visitors_uagent_day;
CREATE TABLE psdb.ag_visitors_uagent_day (
  user_agent_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent_id,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_day_aux;
CREATE TABLE psdb.ag_visitors_uagent_day_aux (
  user_agent_id int(255) NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_month;
CREATE TABLE psdb.ag_visitors_uagent_month (
  user_agent_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent_id,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_month_aux;
CREATE TABLE psdb.ag_visitors_uagent_month_aux (
  user_agent_id int(255) NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_year;
CREATE TABLE psdb.ag_visitors_uagent_year (
  user_agent_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent_id,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_year_aux;
CREATE TABLE psdb.ag_visitors_uagent_year_aux (
  user_agent_id int(255) NOT NULL,
  user_agent_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_visitors_dom_path_day;
CREATE TABLE  psdb.ag_visitors_dom_path_day (
  url_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url_id,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_path_day_aux;
CREATE TABLE  psdb.ag_visitors_dom_path_day_aux (
  url_id int(255) NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_visitors_dom_path_month;
CREATE TABLE  psdb.ag_visitors_dom_path_month (
  url_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url_id,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_path_month_aux;
CREATE TABLE  psdb.ag_visitors_dom_path_month_aux (
  url_id int(255) NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_visitors_dom_path_year;
CREATE TABLE  psdb.ag_visitors_dom_path_year (
  url_id int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url_id,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_path_year_aux;
CREATE TABLE  psdb.ag_visitors_dom_path_year_aux (
  url_id int(255) NOT NULL,
  dom_desc varchar(1000) NOT NULL,
  path_desc varchar(1000) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;


DROP TABLE IF EXISTS psdb.ag_visitors_ccat_day;
CREATE TABLE  psdb.ag_visitors_ccat_day (
  content_category int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,day,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_day_aux;
CREATE TABLE  psdb.ag_visitors_ccat_day_aux (
  content_category int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_month;
CREATE TABLE  psdb.ag_visitors_ccat_month (
  content_category int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,month,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_month_aux;
CREATE TABLE  psdb.ag_visitors_ccat_month_aux (
  content_category int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_year;
CREATE TABLE  psdb.ag_visitors_ccat_year (
  content_category int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,year)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_year_aux;
CREATE TABLE  psdb.ag_visitors_ccat_year_aux (
  content_category int(255) NOT NULL,
  protocol varchar(20) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;