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
