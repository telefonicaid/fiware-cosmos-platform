package es.tid.cosmos.base.mapreduce;

import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import org.junit.Test;

/**
 *
 * @author ximo
 */
public class OutputEraserTest {
    @Test
    public void testGet() {
        assertNull(OutputEraser.getEraser(this.getClass()));
        assertEquals(OutputEraser.getEraser(FileOutputFormat.class).getClass(),
                     FileDataEraser.class);
    }
}
