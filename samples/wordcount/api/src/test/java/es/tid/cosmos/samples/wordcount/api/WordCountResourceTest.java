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

import static org.junit.Assert.assertEquals;
import org.junit.Test;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * @author sortega
 */
public class WordCountResourceTest {
    @Test
    public void testGetCount() throws Exception {
        WordCountResource instance = new WordCountResource();
        WordCountDAO daoMock = mock(WordCountDAO.class);
        when(daoMock.getCount("word")).thenReturn(10L);
        instance.setDao(daoMock);
        assertEquals(10L, instance.getCount("word"));
    }
}
