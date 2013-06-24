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

package es.tid.cosmos.injection.sftp.hadoopfs;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.log4j.Logger;
import org.apache.sshd.common.Session;
import org.apache.sshd.server.FileSystemFactory;
import org.apache.sshd.server.FileSystemView;

/**
 * HadoopFileSystemFactory
 *
 * @author logc
 */
public class HadoopFileSystemFactory implements FileSystemFactory {

    private static final Logger LOGGER =
            Logger.getLogger(HadoopFileSystemFactory.class);

    private final Configuration configuration;

    public HadoopFileSystemFactory(Configuration configuration) {
        this.configuration = configuration;
    }

    @Override
    public FileSystemView createFileSystemView(Session session)
            throws IOException {
        try {
            return new HadoopFileSystemView(session.getUsername(),
                    this.configuration);
        } catch (InterruptedException e) {
            LOGGER.error(e.getMessage(), e);
            return null;
        }
    }
}
