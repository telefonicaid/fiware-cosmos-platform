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

package es.tid.cosmos.tests.tasks;

import es.tid.cosmos.tests.environment.Environment;

/**
 *
 * @author ximo
 */
public class FrontendTaskCreator implements TaskCreator {
    private final Environment env;

    public FrontendTaskCreator(Environment env) {
        this.env = env;
    }

    @Override
    public Task createTask(String inputFilePath, String jarPath) {
        return new FrontEndTask(this.env, inputFilePath, jarPath);
    }
}
