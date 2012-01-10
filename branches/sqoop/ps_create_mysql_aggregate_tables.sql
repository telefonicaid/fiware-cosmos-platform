/* Script to create all target and auxiliary tables in the mysql database psdb where the export data will be loaded */

/* First check if the database exists */
DROP DATABASE IF EXISTS psdb;
CREATE DATABASE psdb;

/* Create the target table in the usually way. In the next command the auxiliary tables are created like a copy of the target ones */ 
/* Page views tables */
DROP TABLE IF EXISTS psdb.ag_page_views_proto_day;
CREATE TABLE  psdb.ag_page_views_proto_day (
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_proto_day_aux;
create table psdb.ag_page_views_proto_day_aux like psdb.ag_page_views_proto_day;

create table ag_page_views_proto_month_aux like ag_page_views_proto_month;
DROP TABLE IF EXISTS psdb.ag_page_views_proto_month;
CREATE TABLE  psdb.ag_page_views_proto_month (
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_proto_month_aux;
create table psdb.ag_page_views_proto_month_aux like psdb.ag_page_views_proto_month;

DROP TABLE IF EXISTS psdb.ag_page_views_proto_year;
CREATE TABLE  psdb.ag_page_views_proto_year (
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_proto_year_aux;
create table psdb.ag_page_views_proto_year_aux like psdb.ag_page_views_proto_year;

DROP TABLE IF EXISTS psdb.ag_page_views_method_day;
CREATE TABLE  psdb.ag_page_views_method_day (
  method varchar(10) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (method,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_method_day_aux;
create table psdb.ag_page_views_method_day_aux like psdb.ag_page_views_method_day;

DROP TABLE IF EXISTS psdb.ag_page_views_method_month;
CREATE TABLE  psdb.ag_page_views_method_month (
  method varchar(10) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (method,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_method_month_aux;
create table psdb.ag_page_views_method_month_aux like psdb.ag_page_views_method_month;

DROP TABLE IF EXISTS psdb.ag_page_views_method_year;
CREATE TABLE  psdb.ag_page_views_method_year (
  method varchar(10) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (method,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_method_year_aux;
create table psdb.ag_page_views_method_year_aux like psdb.ag_page_views_method_year;


DROP TABLE IF EXISTS psdb.ag_page_views_uagent_day;
CREATE TABLE  psdb.ag_page_views_uagent_day (
  user_agent varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_day_aux;
create table psdb.ag_page_views_uagent_day_aux like psdb.ag_page_views_uagent_day;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_month;
CREATE TABLE  psdb.ag_page_views_uagent_month (
  user_agent varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_month_aux;
create table psdb.ag_page_views_uagent_month_aux like psdb.ag_page_views_uagent_month;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_year;
CREATE TABLE  psdb.ag_page_views_uagent_year (
  user_agent varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_uagent_year_aux;
create table psdb.ag_page_views_uagent_year_aux like psdb.ag_page_views_uagent_year;


DROP TABLE IF EXISTS psdb.ag_page_views_url_day;
CREATE TABLE  psdb.ag_page_views_url_day (
  url text NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url(255),protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_url_day_aux;
create table psdb.ag_page_views_url_day_aux like psdb.ag_page_views_url_day;

DROP TABLE IF EXISTS psdb.ag_page_views_url_month;
CREATE TABLE  psdb.ag_page_views_url_month (
  url text NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url(255),protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_url_month_aux;
create table psdb.ag_page_views_url_month_aux like psdb.ag_page_views_url_month;

DROP TABLE IF EXISTS psdb.ag_page_views_url_year;
CREATE TABLE  psdb.ag_page_views_url_year (
  url text NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url(255),protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_url_year_aux;
create table psdb.ag_page_views_url_year_aux like psdb.ag_page_views_url_year;


DROP TABLE IF EXISTS psdb.ag_page_views_dom_day;
CREATE TABLE  psdb.ag_page_views_dom_day (
  domain varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (domain,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_day_aux;
create table psdb.ag_page_views_dom_day_aux like psdb.ag_page_views_dom_day;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_month;
CREATE TABLE  psdb.ag_page_views_dom_month (
  domain varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (domain,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_month_aux;
create table psdb.ag_page_views_dom_month_aux like psdb.ag_page_views_dom_month;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_year;
CREATE TABLE  psdb.ag_page_views_dom_year (
  domain varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (domain,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_dom_year_aux;
create table psdb.ag_page_views_dom_year_aux like psdb.ag_page_views_dom_year;


DROP TABLE IF EXISTS psdb.ag_page_views_ccat_day;
CREATE TABLE  psdb.ag_page_views_ccat_day (
  content_category varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_day_aux;
create table psdb.ag_page_views_ccat_day_aux like psdb.ag_page_views_ccat_day;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_month;
CREATE TABLE  psdb.ag_page_views_ccat_month (
  content_category varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_month_aux;
create table psdb.ag_page_views_ccat_month_aux like psdb.ag_page_views_ccat_month;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_year;
CREATE TABLE  psdb.ag_page_views_ccat_year (
  content_category varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  views_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_page_views_ccat_year_aux;
create table psdb.ag_page_views_ccat_year_aux like psdb.ag_page_views_ccat_year;


DROP TABLE IF EXISTS psdb.ag_searches_key_sea_day;
CREATE TABLE  psdb.ag_searches_key_sea_day (
  key_sea varchar(200) NOT NULL,
  searcher varchar(200) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL,
  PRIMARY KEY (key_sea,searcher,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_searches_key_sea_day_aux;
create table psdb.ag_searches_key_sea_day_aux like psdb.ag_searches_key_sea_day;

DROP TABLE IF EXISTS psdb.ag_searches_key_sea_month;
CREATE TABLE  psdb.ag_searches_key_sea_month (
  key_sea varchar(200) NOT NULL,
  searcher varchar(200) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL,
  PRIMARY KEY (key_sea,searcher,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_searches_key_sea_month_aux;
create table psdb.ag_searches_key_sea_month_aux like psdb.ag_searches_key_sea_month;

DROP TABLE IF EXISTS psdb.ag_searches_key_sea_year;
CREATE TABLE  psdb.ag_searches_key_sea_year (
  key_sea varchar(200) NOT NULL,
  searcher varchar(200) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  searches_num int(10) unsigned NOT NULL,
  PRIMARY KEY (key_sea,searcher,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_searches_key_sea_year_aux;
create table psdb.ag_searches_key_sea_year_aux like psdb.ag_searches_key_sea_year;

/* Visitors tables */
DROP TABLE IF EXISTS psdb.ag_visitors_proto_day;
CREATE TABLE psdb.ag_visitors_proto_day (
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_day_aux;
create table psdb.ag_visitors_proto_day_aux like psdb.ag_visitors_proto_day;

create table ag_visitors_proto_month_aux like ag_visitors_proto_month;
DROP TABLE IF EXISTS psdb.ag_visitors_proto_month;
CREATE TABLE  psdb.ag_visitors_proto_month (
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_month_aux;
create table psdb.ag_visitors_proto_month_aux like psdb.ag_visitors_proto_month;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_year;
CREATE TABLE  psdb.ag_visitors_proto_year (
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_proto_year_aux;
create table psdb.ag_visitors_proto_year_aux like psdb.ag_visitors_proto_year;


DROP TABLE IF EXISTS psdb.ag_visitors_uagent_day;
CREATE TABLE psdb.ag_visitors_uagent_day (
  user_agent varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_day_aux;
create table psdb.ag_visitors_uagent_day_aux like psdb.ag_visitors_uagent_day;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_month;
CREATE TABLE psdb.ag_visitors_uagent_month (
  user_agent varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_month_aux;
create table psdb.ag_visitors_uagent_month_aux like psdb.ag_visitors_uagent_month;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_year;
CREATE TABLE psdb.ag_visitors_uagent_year (
  user_agent varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (user_agent,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_uagent_year_aux;
create table psdb.ag_visitors_uagent_year_aux like psdb.ag_visitors_uagent_year;


DROP TABLE IF EXISTS psdb.ag_visitors_url_day;
CREATE TABLE  psdb.ag_visitors_url_day (
  url text NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url(255),protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_url_day_aux;
create table psdb.ag_visitors_url_day_aux like psdb.ag_visitors_url_day;

DROP TABLE IF EXISTS psdb.ag_visitors_url_month;
CREATE TABLE  psdb.ag_visitors_url_month (
  url text NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url(255),protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_url_month_aux;
create table psdb.ag_visitors_url_month_aux like psdb.ag_visitors_url_month;

DROP TABLE IF EXISTS psdb.ag_visitors_url_year;
CREATE TABLE  psdb.ag_visitors_url_year (
  url text NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (url(255),protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_url_year_aux;
create table psdb.ag_visitors_url_year_aux like psdb.ag_visitors_url_year;


DROP TABLE IF EXISTS psdb.ag_visitors_dom_day;
CREATE TABLE  psdb.ag_visitors_dom_day (
  domain varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (domain,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_day_aux;
create table psdb.ag_visitors_dom_day_aux like psdb.ag_visitors_dom_day;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_month;
CREATE TABLE  psdb.ag_visitors_dom_month (
  domain varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (domain,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_month_aux;
create table psdb.ag_visitors_dom_month_aux like psdb.ag_visitors_dom_month;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_year;
CREATE TABLE  psdb.ag_visitors_dom_year (
  domain varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (domain,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_dom_year_aux;
create table psdb.ag_visitors_dom_year_aux like psdb.ag_visitors_dom_year;


DROP TABLE IF EXISTS psdb.ag_visitors_ccat_day;
CREATE TABLE  psdb.ag_visitors_ccat_day (
  content_category varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  day int(2) unsigned NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,day,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_day_aux;
create table psdb.ag_visitors_ccat_day_aux like psdb.ag_visitors_ccat_day;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_month;
CREATE TABLE  psdb.ag_visitors_ccat_month (
  content_category varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  month int(2) unsigned NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,month,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_month_aux;
create table psdb.ag_visitors_ccat_month_aux like psdb.ag_visitors_ccat_month;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_year;
CREATE TABLE  psdb.ag_visitors_ccat_year (
  content_category varchar(255) NOT NULL,
  protocol varchar(10) NOT NULL,
  year int(4) unsigned NOT NULL,
  visitors_num int(10) unsigned NOT NULL,
  PRIMARY KEY (content_category,protocol,year)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS psdb.ag_visitors_ccat_year_aux;
create table psdb.ag_visitors_ccat_year_aux like psdb.ag_visitors_ccat_year;