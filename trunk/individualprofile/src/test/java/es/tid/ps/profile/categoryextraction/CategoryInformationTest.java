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
package es.tid.ps.profile.categoryextraction;

import java.io.*;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

/**
 * Use case for CategoryInformation
 *
 * @author sortega@tid.es
 */
public class CategoryInformationTest {
    private CategoryInformation instance;

    @Before
    public void setUp() {
        instance = new CategoryInformation("XX0001",
                "http://www.google.com/weather", "02/01/2012",
                128, new String[] {"SPORTS", "LIFESTYLE"});
    }

    @Test
    public void testSerializationRoundTrip() throws Exception {
        // Serialization
        ByteArrayOutputStream bytes = new ByteArrayOutputStream();
        DataOutput output = new DataOutputStream(bytes);
        instance.write(output);

        // Deserialization
        DataInput input = new DataInputStream(
                new ByteArrayInputStream(bytes.toByteArray()));
        CategoryInformation copy = new CategoryInformation();
        copy.readFields(input);

        assertEquals(instance, copy);
    }
}
