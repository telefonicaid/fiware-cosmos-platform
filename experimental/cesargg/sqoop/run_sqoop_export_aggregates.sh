#!/bin/bash

# Actions to be performed in the Oozie workflow. For each file in hdfs, the corresponding aux table must be cleaned before the data are loaded

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_proto_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_proto_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_proto_day --export-dir /user/cesargg/sqoop/ag_page_views_proto_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_proto_month"
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "insert into ag_page_views_proto_month select protocol, month, year, sum(views_num) from ag_page_views_proto_day group by protocol, month, year"
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_proto_year"
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "insert into ag_page_views_proto_year select protocol, year, sum(views_num) from ag_page_views_proto_month group by protocol, year"

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_method_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_method_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_method_day --export-dir /user/cesargg/sqoop/ag_page_views_method_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_method_month_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_method_month_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_method_month --export-dir /user/cesargg/sqoop/ag_page_views_method_month
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_method_year_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_method_year_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_method_year --export-dir /user/cesargg/sqoop/ag_page_views_method_year

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_uagent_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_uagent_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_uagent_day --export-dir /user/cesargg/sqoop/ag_page_views_uagent_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_uagent_month_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_uagent_month_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_uagent_month --export-dir /user/cesargg/sqoop/ag_page_views_uagent_month
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_uagent_year_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_uagent_year_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_uagent_year --export-dir /user/cesargg/sqoop/ag_page_views_uagent_year

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_url_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_url_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_url_day --export-dir /user/cesargg/sqoop/ag_page_views_url_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_url_month_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_url_month_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_url_month --export-dir /user/cesargg/sqoop/ag_page_views_url_month
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_url_year_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_url_year_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_url_year --export-dir /user/cesargg/sqoop/ag_page_views_url_year

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_dom_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_dom_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_dom_day --export-dir /user/cesargg/sqoop/ag_page_views_dom_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_dom_month_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_dom_month_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_dom_month --export-dir /user/cesargg/sqoop/ag_page_views_dom_month
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_dom_year_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_dom_year_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_dom_year --export-dir /user/cesargg/sqoop/ag_page_views_dom_year

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_ccat_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_ccat_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_ccat_day --export-dir /user/cesargg/sqoop/ag_page_views_ccat_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_ccat_month_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_ccat_month_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_ccat_month --export-dir /user/cesargg/sqoop/ag_page_views_ccat_month
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_page_views_ccat_year_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_ccat_year_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_page_views_ccat_year --export-dir /user/cesargg/sqoop/ag_page_views_ccat_year

sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_searches_key_sea_day_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_searches_key_sea_day_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_searches_key_sea_day --export-dir /user/cesargg/sqoop/ag_searches_key_sea_day
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_searches_key_sea_month_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_searches_key_sea_month_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_searches_key_sea_month --export-dir /user/cesargg/sqoop/ag_searches_key_sea_month
sqoop eval --connect jdbc:mysql://pshdp01/psdb --username hadoop --password hadoop -e "truncate table ag_searches_key_sea_year_aux"
sqoop export --connect jdbc:mysql://pshdp01/psdb --table ag_searches_key_sea_year_aux --username hadoop --password hadoop --jar-file ./aggregates_web_profiling.jar --class-name ag_searches_key_sea_year --export-dir /user/cesargg/sqoop/ag_searches_key_sea_year