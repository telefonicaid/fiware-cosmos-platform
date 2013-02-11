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

import java.io.IOException;
import java.net.URI;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.sshd.server.FileSystemView;
import org.apache.sshd.server.SshFile;

import es.tid.cosmos.base.util.Logger;

/**
 * HadoopFileSystemView
 *
 * @author logc
 * @since  CTP 2
 */
public class HadoopFileSystemView implements FileSystemView {
    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(HadoopFileSystemView.class);

    private String homePath;
    private FileSystem hadoopFS;
    private final String userName;

    public HadoopFileSystemView(String userName, Configuration configuration)
            throws IOException, InterruptedException {
        this.userName = userName;
        try {
            this.hadoopFS = FileSystem.get(
                    URI.create(configuration.get("fs.default.name")),
                    configuration, this.userName);
            this.homePath = this.hadoopFS.getHomeDirectory().toString()
                    .replaceFirst(this.hadoopFS.getUri().toString(), "");
        } catch (IOException e) {
            LOGGER.error(e.getMessage(), e);
            throw e;
        } catch (InterruptedException e) {
            LOGGER.error(e.getMessage(), e);
            throw e;
        }
    }

    @Override
    public HadoopSshFile getFile(String file) {
        try {
            return this.getFile("", file);
        } catch (IOException e) {
            LOGGER.error(e.getMessage(), e);
        } catch (InterruptedException e) {
            LOGGER.error(e.getMessage(), e);
        }
        return null;
    }

    @Override
    public HadoopSshFile getFile(SshFile baseDir, String file) {
        try {
            return this.getFile(baseDir.getAbsolutePath(), file);
        } catch (IOException e) {
            LOGGER.error(e.getMessage(), e);
        } catch (InterruptedException e) {
            LOGGER.error(e.getMessage(), e);
        }
        return null;
    }

    private HadoopSshFile getFile(String baseDir, String file)
            throws IOException, InterruptedException {
        String requestedDir = baseDir;
        String requestedFile = file;
        if (requestedDir.isEmpty() && (requestedFile.isEmpty() ||
                requestedFile.equals(Path.CUR_DIR))) {
            requestedDir = this.homePath;
            requestedFile = "";
            LOGGER.debug("redirecting to home path: " + this.homePath);
        }
        String wholePath = requestedDir + requestedFile;
        if (!requestedDir.endsWith(Path.SEPARATOR) &&
                !requestedFile.startsWith(Path.SEPARATOR)) {
            wholePath = requestedDir + Path.SEPARATOR + requestedFile;
        }
        return new HadoopSshFile(wholePath, this.userName, this.hadoopFS);
    }
}
