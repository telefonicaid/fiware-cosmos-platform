package es.tid.cosmos.tests.hadoopjars;

import java.io.IOException;
import java.util.Properties;

import static org.testng.Assert.fail;

/**
 *
 * @author ximo
 */
public class HadoopJars {
    private static final Properties props;

    static {
        props = new Properties();
        try {
            props.load(HadoopJars.class.getResource("/jarfiles.properties").openStream());
        } catch (IOException ex) {
            fail("IOException while loading configuration: " + ex.toString());
        }
    }

    private HadoopJars() {
    }

    public static String getPath(JarNames jar) {
        return props.getProperty(jar.toString());
    }
}
