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

package es.tid.cosmos.profile.categoryextraction;

import java.io.IOException;
import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.cosmos.base.mapreduce.TernaryKey;
import es.tid.cosmos.profile.dictionary.Categorization;
import es.tid.cosmos.profile.dictionary.Dictionary;
import es.tid.cosmos.profile.dictionary.comscore.DistributedCacheDictionary;
import es.tid.cosmos.profile.generated.data.ProfileProtocol.CategoryInformation;

/**
 * Reduces <[userId, date, url], count> into <[userId, date], [count,
 * categories]>
 *
 * @author dmicol, sortega
 */
public class CategoryExtractionReducer extends Reducer<TernaryKey,
        LongWritable, BinaryKey, ProtobufWritable<CategoryInformation>> {
    private static Dictionary sharedDictionary = null;
    private ProtobufWritable<CategoryInformation> catWrapper;
    private BinaryKey outKey;

    @Override
    public void setup(Context context) throws IOException {
        this.setupDictionary(context);
        this.catWrapper = new ProtobufWritable<CategoryInformation>();
        this.catWrapper.setConverter(CategoryInformation.class);
        this.outKey = new BinaryKey();
    }

    protected void setupDictionary(Context context) throws IOException {
        if (sharedDictionary == null) {
            sharedDictionary = DistributedCacheDictionary
                    .loadFromCache(context.getConfiguration());
        }
    }

    @Override
    protected void reduce(TernaryKey inKey, Iterable<LongWritable> counts,
            Context context)
            throws IOException, InterruptedException {
        long urlInstances = getCount(counts);
        String url = inKey.getTertiaryKey();
        Categorization dictionaryResponse = this.categorize(url);

        switch (dictionaryResponse.getResult()) {
            case KNOWN_URL:
                context.getCounter(CategoryExtractionCounter.KNOWN_VISITS)
                        .increment(urlInstances);
                this.catWrapper.set(CategoryInformation.newBuilder()
                        .setUserId(inKey.getPrimaryKey())
                        .setUrl(url)
                        .setDate(inKey.getSecondaryKey())
                        .setCount(urlInstances)
                        .addAllCategories(asList(dictionaryResponse
                                .getCategories()))
                        .build());
                this.outKey.setPrimaryKey(inKey.getPrimaryKey());
                this.outKey.setSecondaryKey(inKey.getSecondaryKey());
                context.write(this.outKey, this.catWrapper);
                break;

            case IRRELEVANT_URL:
                context.getCounter(CategoryExtractionCounter.IRRELEVANT_VISITS)
                        .increment(urlInstances);
                break;

            case UNKNOWN_URL:
                // TODO: do something smart for URL category extraction.
                context.getCounter(CategoryExtractionCounter.UNKNOWN_VISITS)
                        .increment(urlInstances);
                break;

            case GENERIC_FAILURE:
                context.getCounter(CategoryExtractionCounter.UNPROCESSED_VISITS)
                        .increment(urlInstances);
                break;

            default:
                throw new IllegalStateException("Invalid dictionary response.");
        }
    }

    protected Categorization categorize(String url) {
        return sharedDictionary.categorize(url);
    }

    private long getCount(Iterable<LongWritable> counts) {
        long urlInstances = 0L;
        for (LongWritable count : counts) {
            urlInstances += count.get();
        }
        return urlInstances;
    }
}
