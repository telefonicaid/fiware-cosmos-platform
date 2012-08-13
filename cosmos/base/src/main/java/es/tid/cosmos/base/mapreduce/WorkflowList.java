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
import java.util.LinkedList;

/**
 * This is a simple extension of LinkedList<CosmosWorkflow> that implements
 * CosmosWorkflow. This class that will call all jobs in the list in parallel
 * (by submitting them all before waiting for them)
 *
 * @author ximo
 */
public class WorkflowList extends LinkedList<CosmosWorkflow>
        implements CosmosWorkflow {
    private volatile ExceptionedThread submittedThread = null;
    private WorkflowList dependencies = null;

    /**
     * This will call submit on all workflows in the list and then call
     * waitForCompletion on each of them.
     * @param verbose print the progress to the user
     * @throws IOException thrown if the communication with the JobTracker is lost
     * @throws InterruptedException
     * @throws ClassNotFoundException
     */
    @Override
    public boolean waitForCompletion(boolean verbose)
            throws IOException, InterruptedException, ClassNotFoundException {
        this.submit(verbose);
        this.submittedThread.join();
        this.submittedThread.throwErrors();
        return true;
    }

    @Override
    public void submit() {
        this.submit(true);
    }

    private synchronized void submit(final boolean verbose) {
        if (this.submittedThread != null) {
            return;
        }

        // Create thread for current job
        this.submittedThread = new ExceptionedThread() {
            @Override
            public void run() {
                try {
                    if (WorkflowList.this.dependencies != null) {
                        WorkflowList.this.dependencies.waitForCompletion(verbose);
                    }
                    for (CosmosWorkflow wf : WorkflowList.this) {
                        wf.submit();
                    }
                    for (CosmosWorkflow wf : WorkflowList.this) {
                        wf.waitForCompletion(verbose);
                    }
                } catch (Exception ex) {
                    this.setException(ex);
                }
            }
        };

        this.submittedThread.start();
    }

    @Override
    public void addDependentWorkflow(CosmosWorkflow wf) {
        if (this.submittedThread != null) {
            throw new IllegalStateException("Cannot add a dependent workflow to a"
                    + "WorkflowList if it has been already submitted");
        }
        if (this.equals(wf)) {
            throw new IllegalArgumentException("Cannot add a workflow to its own"
                    + "dependent workflow list.");
        }
        if (wf != null) {
            if (this.dependencies == null) {
                this.dependencies = new WorkflowList();
            }
            this.dependencies.add(wf);
        }
    }
}