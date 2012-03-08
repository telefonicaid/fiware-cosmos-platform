package org.apache.mahout.tid.impl.recommender.hadoop.preparation;

/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.SequenceFileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.SequenceFileOutputFormat;
import org.apache.hadoop.util.ToolRunner;
import org.apache.mahout.common.AbstractJob;
import org.apache.mahout.common.HadoopUtil;
import org.apache.mahout.cf.taste.hadoop.EntityPrefWritable;
import org.apache.mahout.tid.hadoop.EventsWritable;
import org.apache.mahout.tid.hadoop.ToEventPrefsMapper;
import org.apache.mahout.tid.hadoop.ToItemPrefsMapper;
import org.apache.mahout.tid.hadoop.item.ItemIDIndexMapper;
import org.apache.mahout.tid.hadoop.item.ItemIDIndexReducer;
import org.apache.mahout.tid.hadoop.item.ToUserVectorsReducer;
import org.apache.mahout.tid.impl.recommender.RecommenderTIDJob;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.PreparePreferenceMatrixJob;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.ToBlackListMapper;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.ToBlackListReducer;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.ToItemVectorsMapper;
import org.apache.mahout.tid.impl.recommender.hadoop.preparation.ToItemVectorsReducer;

import org.apache.mahout.math.VarIntWritable;
import org.apache.mahout.math.VarLongWritable;
import org.apache.mahout.math.VectorWritable;

import com.mongodb.BasicDBObject;
import com.mongodb.hadoop.*;
import com.mongodb.hadoop.util.MongoConfigUtil;

import java.util.List;
import java.util.Map;

public class PreparePreferenceMatrixJob extends AbstractJob {

    public static final String NUM_USERS = "numUsers.bin";
    public static final String ITEMID_INDEX = "itemIDIndex";
    public static final String USER_VECTORS = "userVectors";
    public static final String RATING_MATRIX = "ratingMatrix";
    public static final String BLACK_LIST = "blacklist";
    public static final String EVENTS = "events";
    public static final String EVENTSW = "eventsw";

    public static String mongoIn;
    public static String mongoOut;
    public static String mongoCatalog;
    public static String mongoBL;
    public static BasicDBObject query_users;
    public static BasicDBObject query_items;
    public static boolean mongoInput = false;

    private static final int DEFAULT_MIN_PREFS_PER_USER = 1;

    public static void main(String[] args) throws Exception {
        ToolRunner.run(new PreparePreferenceMatrixJob(), args);
    }

    @Override
    public int run(String[] args) throws Exception {

        addInputOption();
        addOutputOption();
        addOption("maxPrefsPerUser", "mppu",
                "max number of preferences to consider per user, "
                        + "users with more preferences will be sampled down");
        addOption("minPrefsPerUser", "mp",
                "ignore users with less preferences than this " + "(default: "
                        + DEFAULT_MIN_PREFS_PER_USER + ')',
                String.valueOf(DEFAULT_MIN_PREFS_PER_USER));
        addOption("booleanData", "b", "Treat input as without pref values",
                Boolean.FALSE.toString());
        addOption("mongo_in_uri", "mui", "Mongo INPUT URI",
                Boolean.FALSE.toString());
        addOption("mongo_out_uri", "muo", "Mongo OUTPUT URI",
                Boolean.FALSE.toString());
        addOption("mongo_in_uri_catalog", "muic", "Mongo INPUT CATALOG  URI",
                Boolean.FALSE.toString());
        addOption("mongo_black_list", "muic", "Mongo BlackList  URI",
                Boolean.FALSE.toString());
        addOption("mongoInput", "mI", "Mongo Input", Boolean.FALSE.toString());
        addOption("tempDir", "t", "temporal folder", Boolean.FALSE.toString());
        addOption("ratingShift", "rs", "shift ratings by this value", "0.0");

        Map<String, List<String>> parsedArgs = parseArguments(args, true, true);
        if (parsedArgs == null) {
            return -1;
        }
        int minPrefsPerUser = Integer.parseInt(parsedArgs.get(
                "--minPrefsPerUser").get(0));
        boolean booleanData = Boolean.valueOf(parsedArgs.get("--booleanData")
                .get(0));
        boolean mongoInput = Boolean.valueOf(parsedArgs.get("--mongoInput")
                .get(0));

        if (mongoInput) {
            mongoIn = parsedArgs.get("--mongo_in_uri").get(0);
            mongoOut = parsedArgs.get("--mongo_out_uri").get(0);
            mongoCatalog = parsedArgs.get("--mongo_in_uri_catalog").get(0);
            mongoBL = parsedArgs.get("--mongo_black_list").get(0);

            // convert items to an internal index
            query_users = new BasicDBObject();
            query_users.put("user_id", new BasicDBObject("$ne", "0"));

            query_items = new BasicDBObject();
            query_items.put("item_id", new BasicDBObject("$ne", "0"));

            MongoConfigUtil.setInputURI(getConf(), mongoCatalog);
            MongoConfigUtil.setOutputURI(getConf(), mongoOut);
            MongoConfigUtil.setQuery(getConf(), query_items);

            Job itemIDIndex = prepareJob(getOutputPath(ITEMID_INDEX),
                    getOutputPath(ITEMID_INDEX), MongoInputFormat.class,
                    ItemIDIndexMapper.class, VarIntWritable.class,
                    VarLongWritable.class, ItemIDIndexReducer.class,
                    VarIntWritable.class, VarLongWritable.class,
                    SequenceFileOutputFormat.class);

            itemIDIndex.setCombinerClass(ItemIDIndexReducer.class);
            itemIDIndex.waitForCompletion(true);

            // convert user preferences into a vector per user
            MongoConfigUtil.setInputURI(getConf(), mongoIn);
            MongoConfigUtil.setInputKey(getConf(), "user_id");
            MongoConfigUtil.setQuery(getConf(), query_users);

            Job toEventsFile = prepareJob(getOutputPath(EVENTS),
                    getOutputPath(EVENTSW), MongoInputFormat.class,
                    ToEventPrefsMapper.class, VarLongWritable.class,
                    EventsWritable.class, SequenceFileOutputFormat.class);

            toEventsFile.waitForCompletion(true);
        } else {
            Job toEventsFile = prepareJob(getInputPath(),
                    getOutputPath(EVENTSW), TextInputFormat.class,
                    ToEventPrefsMapper.class, VarLongWritable.class,
                    EventsWritable.class, SequenceFileOutputFormat.class);

            toEventsFile.waitForCompletion(true);

            Job itemIDIndex = prepareJob(getOutputPath(EVENTSW),
                    getOutputPath(ITEMID_INDEX), TextInputFormat.class,
                    ItemIDIndexMapper.class, VarIntWritable.class,
                    VarLongWritable.class, ItemIDIndexReducer.class,
                    VarIntWritable.class, VarLongWritable.class,
                    SequenceFileOutputFormat.class);

            itemIDIndex.setCombinerClass(ItemIDIndexReducer.class);
        }
        // convert user preferences into a vector per user
        Job toUserVectors = prepareJob(getOutputPath(EVENTSW),
                getOutputPath(USER_VECTORS), SequenceFileInputFormat.class,
                ToItemPrefsMapper.class, VarLongWritable.class,
                booleanData ? VarLongWritable.class : EntityPrefWritable.class,
                ToUserVectorsReducer.class, VarLongWritable.class,
                VectorWritable.class, SequenceFileOutputFormat.class);

        toUserVectors.getConfiguration().setBoolean(
                RecommenderTIDJob.BOOLEAN_DATA, booleanData);
        toUserVectors.getConfiguration().setInt(
                ToUserVectorsReducer.MIN_PREFERENCES_PER_USER, minPrefsPerUser);

        toUserVectors.waitForCompletion(true);

        if (mongoInput) {
            MongoConfigUtil.setOutputURI(getConf(), mongoBL);

            Job toBlackList = prepareJob(getOutputPath(EVENTSW),
                    getOutputPath("blacklist"), SequenceFileInputFormat.class,
                    ToBlackListMapper.class, VarLongWritable.class,
                    VarLongWritable.class, ToBlackListReducer.class,
                    VarLongWritable.class, VarLongWritable.class,
                    MongoOutputFormat.class);

            toBlackList.waitForCompletion(true);
        }

        // we need the number of users later
        int numberOfUsers = (int) toUserVectors.getCounters()
                .findCounter(ToUserVectorsReducer.Counters.USERS).getValue();

        HadoopUtil.writeInt(numberOfUsers, getOutputPath(NUM_USERS), getConf());
        // build the rating matrix
        Job toItemVectors = prepareJob(getOutputPath(USER_VECTORS),
                getOutputPath(RATING_MATRIX), ToItemVectorsMapper.class,
                IntWritable.class, VectorWritable.class,
                ToItemVectorsReducer.class, IntWritable.class,
                VectorWritable.class);

        toItemVectors.setCombinerClass(ToItemVectorsReducer.class);
        /* configure sampling regarding the uservectors */
        if (parsedArgs.containsKey("--maxPrefsPerUser")) {
            int samplingSize = Integer.parseInt(parsedArgs.get(
                    "--maxPrefsPerUser").get(0));
            toItemVectors.getConfiguration().setInt(
                    ToItemVectorsMapper.SAMPLE_SIZE, samplingSize);
        }

        toItemVectors.waitForCompletion(true);
        return 0;
    }
}
