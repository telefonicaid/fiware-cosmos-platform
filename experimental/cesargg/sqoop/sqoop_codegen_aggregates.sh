#!/bin/bash

# Script to generate the classes containing the wrapper to manage database records in lzo format. In the end of the process the class are packet in a jar file

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_proto_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_proto_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_proto_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_method_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_method_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_method_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_uagent_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_uagent_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_uagent_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_url_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_url_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_url_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_dom_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_dom_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_dom_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_ccat_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_ccat_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_page_views_ccat_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_searches_key_sea_day --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_searches_key_sea_month --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .
sqoop import --connect jdbc:mysql://pshdp01/psdb --table ag_searches_key_sea_year --warehouse-dir /user/cesargg/sqoop --compression-codec com.hadoop.compression.lzo.LzoCodec --username hadoop --password hadoop --bindir . --outdir .

jar -cvf aggregates_web_profiling.jar *.class