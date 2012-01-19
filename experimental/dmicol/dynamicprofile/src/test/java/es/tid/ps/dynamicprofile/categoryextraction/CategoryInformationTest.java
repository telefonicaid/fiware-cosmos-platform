// <editor-fold defaultstate="collapsed" desc="Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.">
//
//   File        : CategoryInformationTest.java
//
//   Copyright © 2012 Telefónica Investigación y Desarrollo S.A.U.
//
//   The copyright to the file(s) is property of Telefonica I+D.
//   The file(s) may be used and or copied only with the express written
//   consent of Telefonica I+D or in accordance with the terms and conditions
//   stipulated in the agreement/contract under which the files(s) have
//   been supplied.
//
// </editor-fold>
package es.tid.ps.dynamicprofile.categoryextraction;

import org.junit.Ignore;
import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataInput;
import java.io.ByteArrayOutputStream;
import java.io.DataOutput;
import java.io.DataOutputStream;
import org.apache.hadoop.io.file.tfile.ByteArray;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 * @author sortega@tid.es
 */
public class CategoryInformationTest {
    private CategoryInformation instance;

    @Test
    public void testWrite() throws Exception {
        instance = new CategoryInformation("XX0001",
                "http://www.google.com/weather", 128,
                new String[] {"SPORTS", "LIFESTYLE"});
        ByteArrayOutputStream bytes = new ByteArrayOutputStream();
        DataOutput output = new DataOutputStream(bytes);

        instance.write(output);

        assertEquals(
                "XX0001\thttp://www.google.com/weather\t128\tSPORTS\tLIFESTYLE\n",
                bytes.toString("UTF-16"));
    }

    @Test @Ignore
    public void testRead() throws Exception {
        byte[] bytes = "XX0001\thttp://www.google.com/weather\t128\tSPORTS\tLIFESTYLE\n"
                .getBytes("UTF-16");
        DataInput input = new DataInputStream(
                new ByteArrayInputStream(bytes, 2, bytes.length));

        instance = new CategoryInformation();
        instance.readFields(input);

        assertEquals("XX0001", instance.getUserId());
        assertEquals("http://www.google.com/weather", instance.getUrl());
        assertEquals(128l, instance.getCount());
        assertArrayEquals(new String[] {"SPORTS", "LIFESTYLE"},
                instance.getCategoryNames());
    }
}
