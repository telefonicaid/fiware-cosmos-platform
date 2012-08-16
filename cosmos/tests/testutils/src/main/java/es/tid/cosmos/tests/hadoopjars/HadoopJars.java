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
