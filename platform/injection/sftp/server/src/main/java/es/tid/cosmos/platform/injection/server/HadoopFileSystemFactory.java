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

package es.tid.cosmos.platform.injection.server;

import java.io.IOException;

import es.tid.cosmos.base.util.Logger;
import org.apache.hadoop.conf.Configuration;
import org.apache.sshd.common.Session;
import org.apache.sshd.server.FileSystemFactory;
import org.apache.sshd.server.FileSystemView;

/**
 * HadoopFileSystemFactory
 *
 * @author logc
 * @since  CTP 2
 */
public class HadoopFileSystemFactory implements FileSystemFactory {
    private static final org.apache.log4j.Logger LOG =
            Logger.get(HadoopFileSystemFactory.class);

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
            LOG.error(e.getMessage(), e);
            return null;
        }
    }
}
