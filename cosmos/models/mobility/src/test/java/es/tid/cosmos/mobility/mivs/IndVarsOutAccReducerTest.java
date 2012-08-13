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

package es.tid.cosmos.mobility.mivs;

import java.io.IOException;

import static java.util.Arrays.asList;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.ConfiguredTest;
import es.tid.cosmos.mobility.data.MobVarsUtil;
import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;

/**
 *
 * @author logc
 */
public class IndVarsOutAccReducerTest extends ConfiguredTest {
    private ReduceDriver<LongWritable, TypedProtobufWritable<MobViMobVars>,
            NullWritable, Text> reducer;

    @Before
    public void setUp() throws IOException {
        this.reducer = new ReduceDriver<LongWritable,
                TypedProtobufWritable<MobViMobVars>, NullWritable, Text>(
                        new IndVarsOutAccReducer());
        this.reducer.setConfiguration(this.getConf());
    }

    @Test
    public void testMissingProfilesFilledWithMinusOnes() {
        LongWritable userNotSeenInFirstMonth = new LongWritable(5512684400L);
        MobVars area = MobVarsUtil.create(1, false, 1, 1, 1, 1,
                                          1000000D, 1000000D, 0.0, 0.0);
        TypedProtobufWritable<MobViMobVars> input = new TypedProtobufWritable<MobViMobVars>(
            MobViMobVarsUtil.create(asList(area)));

        String s = "5512684400|-1|-1|-1|-1|-1|-1|-1|-1|1|1|1|1|1000000.0|"
                + "1000000.0|0.0|0.0";
        Text expected = new Text(s);

        this.reducer
                .withInput(userNotSeenInFirstMonth, asList(input))
                .withOutput(NullWritable.get(), expected)
                .runTest();
    }
}
