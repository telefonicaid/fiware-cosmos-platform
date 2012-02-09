package es.tid.ps.mapreduce.mobility;

import java.io.ByteArrayOutputStream;
import java.io.DataOutput;
import java.io.DataOutputStream;
import java.io.IOException;

import junit.framework.TestCase;

import org.apache.hadoop.io.RawComparator;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.BlockJUnit4ClassRunner;

import es.tid.ps.mapreduce.mobility.data.CompositeKey;

/**
 * Test case for NaturalKeySortComparatorTest
 * 
 * @author rgc
 */
@RunWith(BlockJUnit4ClassRunner.class)
public class NaturalKeySortComparatorTest extends TestCase {

    @Test
    public void testCompareCompositeKeyEqual() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 1);
        CompositeKey arg1 = new CompositeKey("1", 1);
        int out = partition.compare(arg0, arg1);
        assertEquals(0, out);
    }

    @Test
    public void testCompareCompositeKeyMUser() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("2", 1);
        CompositeKey arg1 = new CompositeKey("1", 1);
        int out = partition.compare(arg0, arg1);
        assertEquals(1, out);
    }

    @Test
    public void testCompareCompositeKeyMFile() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 2);
        CompositeKey arg1 = new CompositeKey("1", 1);
        int out = partition.compare(arg0, arg1);
        assertEquals(1, out);
    }

    @Test
    public void testCompareCompositeKeymUser() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 1);
        CompositeKey arg1 = new CompositeKey("1", 2);
        int out = partition.compare(arg0, arg1);
        assertEquals(-1, out);
    }

    @Test
    public void testCompareCompositeKeymFile() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 1);
        CompositeKey arg1 = new CompositeKey("2", 1);
        int out = partition.compare(arg0, arg1);
        assertEquals(-1, out);
    }

    @Test
    public void testCompareCompositeKeydFile() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 2);
        CompositeKey arg1 = new CompositeKey("2", 1);
        int out = partition.compare(arg0, arg1);
        assertEquals(-1, out);
    }

    @Test
    public void testCompareCompositeKeyDFile() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("2", 1);
        CompositeKey arg1 = new CompositeKey("1", 2);
        int out = partition.compare(arg0, arg1);
        assertEquals(1, out);
    }

    @Test
    public void testCompareCompositeKeyEqualBinary() {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 1);
        CompositeKey arg1 = new CompositeKey("1", 1);
        int out = partition.compare(arg0, arg1);
        assertEquals(0, out);
    }

    @Test
    public void testCompareCompositeKeyMUserBinary() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("2", 1);
        CompositeKey arg1 = new CompositeKey("1", 1);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutput w = new DataOutputStream(baos);
        arg0.write(w);
        byte[] result = baos.toByteArray();
        arg1.write(w);
        byte[] result2 = baos.toByteArray();

        int out = partition.compare(result, 0, result.length, result2,
                result.length, result2.length);
        assertEquals(1, out);
    }

    @Test
    public void testCompareCompositeKeyMFileBinary() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 2);
        CompositeKey arg1 = new CompositeKey("1", 1);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutput w = new DataOutputStream(baos);
        arg0.write(w);
        byte[] result = baos.toByteArray();
        arg1.write(w);
        byte[] result2 = baos.toByteArray();

        int out = partition.compare(result, 0, result.length, result2,
                result.length, result2.length);
        assertEquals(1, out);
    }

    @Test
    public void testCompareCompositeKeymUserBinary() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 1);
        CompositeKey arg1 = new CompositeKey("1", 2);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutput w = new DataOutputStream(baos);
        arg0.write(w);
        byte[] result = baos.toByteArray();
        arg1.write(w);
        byte[] result2 = baos.toByteArray();

        int out = partition.compare(result, 0, result.length, result2,
                result.length, result2.length);
        assertEquals(-1, out);
    }

    @Test
    public void testCompareCompositeKeymFileBinary() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 1);
        CompositeKey arg1 = new CompositeKey("2", 1);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutput w = new DataOutputStream(baos);
        arg0.write(w);
        byte[] result = baos.toByteArray();
        arg1.write(w);
        byte[] result2 = baos.toByteArray();

        int out = partition.compare(result, 0, result.length, result2,
                result.length, result2.length);
        assertEquals(-1, out);
    }

    @Test
    public void testCompareCompositeKeydFileBinary() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("1", 2);
        CompositeKey arg1 = new CompositeKey("2", 1);

        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutput w = new DataOutputStream(baos);
        arg0.write(w);
        byte[] result = baos.toByteArray();
        arg1.write(w);
        byte[] result2 = baos.toByteArray();

        int out = partition.compare(result, 0, result.length, result2,
                result.length, result2.length);
        assertEquals(-1, out);
    }

    @Test
    public void testCompareCompositeKeyDFileBinary() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        CompositeKey arg0 = new CompositeKey("2", 1);
        CompositeKey arg1 = new CompositeKey("1", 2);
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        DataOutput w = new DataOutputStream(baos);
        arg0.write(w);
        byte[] result = baos.toByteArray();
        arg1.write(w);
        byte[] result2 = baos.toByteArray();

        int out = partition.compare(result, 0, result.length, result2,
                result.length, result2.length);
        assertEquals(1, out);
    }

    @Test(expected = RuntimeException.class)
    public void testCompareCompositeKeyIOexception() throws IOException {
        RawComparator<CompositeKey> partition = new NaturalKeySortComparator();

        byte[] result = new byte[1];
        byte[] result2 = new byte[1];

        partition.compare(result, 0, result.length, result2, result.length,
                result2.length);
    }
}
