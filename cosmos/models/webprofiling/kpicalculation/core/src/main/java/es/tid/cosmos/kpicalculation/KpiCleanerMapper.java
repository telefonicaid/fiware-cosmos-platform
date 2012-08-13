/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

package es.tid.cosmos.kpicalculation;

import java.io.IOException;

import javax.annotation.Resource;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.kpicalculation.cleaning.KpiCalculationFilterChain;
import es.tid.cosmos.kpicalculation.cleaning.KpiCalculationFilterException;
import es.tid.cosmos.kpicalculation.data.KpiCalculationCounter;
import es.tid.cosmos.kpicalculation.data.KpiCalculationDataException;
import es.tid.cosmos.kpicalculation.generated.data.KpiCalculationProtocol.WebProfilingLog;
import es.tid.cosmos.kpicalculation.utils.WebProfilingUtil;

/**
 * This class receives lines of information of CDR´s files that will be used in
 * the Web Profiling module. The lines are parsed to a PageView object, whose
 * fullUrl field will be verified to asses that it passes all the filters
 * defined for the web profiling module in the KpiCalculationFilterChain
 * resource. If the url passes every filter then the PageView string value,
 * which fits the format of PAGE_VIEWS hive's table, will be emited. If the url
 * does not pass any of the filters nothing will be emited. Examples:
 * <ol>
 * <li>Input : {key: 1, values: 16737b1873ef03ad http://www.tid.es/index.html
 * 1Dec2010000001 304 application/pkix-crl -Microsoft-CryptoAPI/6.1 GET}</li>
 * <li>Output: {key: 1, values: 16737b1873ef03ad http http://tid.es/ tid.es /
 * null 1Dec2010000001 1Dec2010000001 -Microsoft-CryptoAPI/6.1
 * -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1 -Microsoft-CryptoAPI/6.1
 * GET 304"}</li>
 * </ol>
 *
 * <ol>
 * <li>Input : {key: 1, values: 16737b1873ef03ad http http://www.tid.es/foto.jpg
 * 1Dec2010000001 304 application/pkix-crl -Microsoft-CryptoAPI/6.1 GET}</li>
 * <li>Output: void</li>
 * </ol>
 *
 *
 * @author javierb@tid.es
 */
public class KpiCleanerMapper extends Mapper<LongWritable, Text, NullWritable,
        ProtobufWritable<WebProfilingLog>> {
    @Resource
    private KpiCalculationFilterChain filter;

    /**
     * Method that prepares the filters to be applied. The classes implementing
     * them and the values to be filtered for each of them will be loaded from
     * the kpi-filtering.xml file
     *
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        context.getConfiguration().addResource("kpi-filtering.xml");
        this.filter = new KpiCalculationFilterChain(context.getConfiguration());
    }

    /**
     * @param key
     *            is the byte offset of the current line in the file;
     * @param value
     *            is the line from the file
     * @param context
     *            has the method "write()" to output the key,value pair
     */
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            WebProfilingLog webLog = WebProfilingUtil.getInstance(value
                    .toString());
            this.filter.filter(webLog.getFullUrl());
            ProtobufWritable<WebProfilingLog> wrapper =
                    new ProtobufWritable<WebProfilingLog>();
            wrapper.setConverter(WebProfilingLog.class);
            wrapper.set(webLog);
            context.getCounter(KpiCalculationCounter.LINE_STORED).increment(1L);
            context.write(NullWritable.get(), wrapper);
        } catch (KpiCalculationFilterException e) {
            context.getCounter(e.getCounter()).increment(1L);
        } catch (KpiCalculationDataException e) {
            context.getCounter(e.getCounter()).increment(1L);
        } catch (Exception e) {
            context.getCounter(KpiCalculationCounter.WRONG_FILTERING_FIELDS)
                    .increment(1L);
        }
    }
}
