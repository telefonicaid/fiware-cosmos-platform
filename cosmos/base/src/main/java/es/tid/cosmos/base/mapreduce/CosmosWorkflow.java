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

package es.tid.cosmos.base.mapreduce;

import java.io.IOException;

/**
 *
 * @author ximo
 */
// TODO: Add support for looping workflows (see AdjacentExtractionRunner in
// mobility model)
public interface CosmosWorkflow {
    /**
     * This method submits and waits for this workflow and all its dependent
     * workflows to finish. It signals errors through JobExecutionException,
     * so the return value will never be false (it is a boolean for Hadoop
     * compatibility reasons)
     * @param verbose print the progress to the user
     * @return true
     * @throws IOException thrown if the communication with the JobTracker is lost
     * @throws InterruptedException
     * @throws ClassNotFoundException
     */
    boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException;

    /**
     * This is a non-blocking method that will run and wait for all the
     * dependent workflows and then start execution of the current workflow
     */
    void submit();

    /**
     * This methods will add the "workflow" parameter to the list of dependent
     * workflows for this class
     * @param workflow
     */
    void addDependentWorkflow(CosmosWorkflow workflow);
}
