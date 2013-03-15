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

package es.tid.cosmos.samples.wordcount.api;

import java.util.ArrayList;
import java.util.List;

import com.mongodb.BasicDBObject;
import com.mongodb.DBObject;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.mock;

/**
 *
 * @author dmicol, sortega
 */
public class WordCountDAOTest {
    private WordCountDAO instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new WordCountDAO() {
            @Override
            public long getCount(String word) {
                List<DBObject> results = new ArrayList<DBObject>();
                if ("test".equals(word)) {
                    results.add(new BasicDBObject("value", 2L));
                } else if ("mutxamel".equals(word)) {
                    results.add(new BasicDBObject("value", 7L));
                    results.add(new BasicDBObject("value", 192L));
                } else {
                    // For all other cases we'll have an empty list
                }
                return this.processResults(results);
            }
        };

        this.instance.setMongo(mock(MongoService.class));
    }

    @Test
    public void shouldRetrieveWordCount() {
        assertEquals(2L, this.instance.getCount("test"));
    }

    @Test
    public void shouldWordNotExist() {
        assertEquals(0L, this.instance.getCount("blah"));
    }

    @Test(expected=IllegalStateException.class)
    public void shouldFail() {
        this.instance.getCount("mutxamel");
    }
}
