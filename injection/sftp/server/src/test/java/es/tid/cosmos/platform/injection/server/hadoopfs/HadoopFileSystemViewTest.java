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

package es.tid.cosmos.platform.injection.server.hadoopfs;

import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.platform.injection.server.BaseSftpTest;
import es.tid.cosmos.platform.injection.server.hadoopfs.HadoopFileSystemView;
import es.tid.cosmos.platform.injection.server.hadoopfs.HadoopSshFile;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertFalse;

/**
 * HadoopFileSystemViewTest
 *
 * @author logc
 */
public class HadoopFileSystemViewTest extends BaseSftpTest {

    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(HadoopFileSystemView.class);

    private String userName;
    private Configuration conf;
    private HadoopFileSystemView hadoopFileSystemView;

    public HadoopFileSystemViewTest() {
        super(LOGGER);
    }

    @Before
    public void setUp() throws Exception {
        this.userName = "test";
        this.conf = new Configuration();
        this.hadoopFileSystemView = new HadoopFileSystemView(this.userName,
                                                             this.conf);
    }

    @Test
    public void testGetFileWithEmptyFilename() throws Exception {
        HadoopSshFile file = this.hadoopFileSystemView.getFile("");
        FileSystem hadoopFS = FileSystem.get(
                URI.create(this.conf.get("fs.default.name")),
                           this.conf, this.userName);
        String homePath = hadoopFS.getHomeDirectory().toString()
                .replaceFirst(hadoopFS.getUri().toString(), "");
        assertEquals(homePath, file.getAbsolutePath());
        assertEquals(homePath.substring(homePath.lastIndexOf("/") + 1),
                     file.getName());
    }

    @Test
    public void testGetFile() throws Exception {
        HadoopSshFile bar = this.hadoopFileSystemView.getFile("/foo/bar");
        assertEquals("bar", bar.getName());
        assertEquals("/foo/bar", bar.getAbsolutePath());
        assertFalse(bar.doesExist());

        HadoopSshFile bq = this.hadoopFileSystemView.getFile(bar, "bq");
        assertEquals("bq", bq.getName());
        assertEquals("/foo/bar/bq", bq.getAbsolutePath());
        assertFalse(bq.doesExist());
    }

    @Test
    public void testRedirectionToHomePath() throws Exception {
        FileSystem fs = FileSystem.get(URI.create(
                this.conf.get("fs.default.name")), this.conf, "test");
        String homePath = fs.getHomeDirectory().toString().replaceFirst(
                fs.getUri().toString(), "");
        HadoopSshFile init = this.hadoopFileSystemView.getFile(
                new HadoopSshFile(".", "test", fs), ".");
        assertEquals(homePath, init.getAbsolutePath());
    }
}
