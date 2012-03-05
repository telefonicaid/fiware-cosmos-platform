package es.tid.bdp.sftp.server.filesystem.hadoop;

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

import java.lang.reflect.Constructor;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.sshd.common.Session;
import org.apache.sshd.server.FileSystemFactory;
import org.apache.sshd.server.FileSystemView;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import es.tid.bdp.utils.FileSystemControllerAbstract;
import es.tid.bdp.utils.PropertiesPlaceHolder;

/**
 * This class opens the connection to HDFS and generates the views to this file
 * system. It reads the properties files, and creates the controller of the
 * file system that it will allow to check the access to the file system,
 * monitoring the permissions and to serialization that it is necessary in each
 * point
 * 
 * @author rgc
 * 
 */
public class HdfsFileSystemFactory implements FileSystemFactory {

    private final Logger LOG = LoggerFactory
            .getLogger(HdfsFileSystemFactory.class);

    private final static String FS_DEFAULT_NAME = "fs.default.name";
    private static final String DESCRIPTOR_BUILDER_CLASS = "filesystem.descriptor.class";

    private boolean caseInsensitive;

    private FileSystem hdfs;
    private FileSystemControllerAbstract hdfsCtrl;

    /**
     * Create the appropriate user file system view.
     */
    public FileSystemView createFileSystemView(Session session) {
        String userName = session.getUsername();
        LOG.debug("Create HDFS File System View for user \"{}\" ", userName);

        prepareHdfsConection();
        FileSystemView fsView = new HdfsFileSystemView(userName,
                caseInsensitive, hdfs, hdfsCtrl);
        return fsView;
    }

    /**
     * Private method. It loads the configuration and prepares its adding the
     * compress codecs an load de fileDescriptors
     */
    private void prepareHdfsConection() {
        try {
            LOG.debug("Loading configurations for HDFS File System View");

            PropertiesPlaceHolder properties = PropertiesPlaceHolder
                    .getInstance();

            Configuration conf = new Configuration();
            conf.set(FS_DEFAULT_NAME, properties.getProperty(FS_DEFAULT_NAME));
            conf.set("io.compression.codecs",
                    "com.hadoop.compression.lzo.LzoCodec,");
            conf.set("io.compression.codec.lzo.class",
                    "com.hadoop.compression.lzo.LzoCodec");

            hdfs = FileSystem.get(conf);

            @SuppressWarnings("unchecked")
            Class<FileSystemControllerAbstract> klass = (Class<FileSystemControllerAbstract>) Class
                    .forName(properties.getProperty(DESCRIPTOR_BUILDER_CLASS));

            Constructor<FileSystemControllerAbstract> constructor = klass
                    .getConstructor(PropertiesPlaceHolder.class);

            hdfsCtrl = constructor.newInstance(properties);
        } catch (Exception e) {
            LOG.error("Error loading the filesystem.", e);
            throw new RuntimeException("Error loading the filesystem", e);
        }
    }
}
