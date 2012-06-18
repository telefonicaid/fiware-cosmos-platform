package es.tid.cosmos.kpicalculation;

import org.apache.hadoop.io.DataOutputBuffer;

import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 * @author sortega
 */
public class BinaryComparatorBaseTest {

    protected byte[] toByteArray(CompositeKey key) throws Exception {
        DataOutputBuffer out = new DataOutputBuffer();
        key.write(out);
        return out.getData();
    }
}
