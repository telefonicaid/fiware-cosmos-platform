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

import org.apache.pig.PigServer;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertTrue;
import static org.mockito.Mockito.mock;

/**
 * @author apv
 */
public class PigScriptJobTest {

    private static class FakeJob implements CosmosWorkflow {

        private boolean completed;

        private FakeJob() {
            this.completed = false;
        }

        @Override
        public boolean waitForCompletion(boolean verbose) {
            this.doWork();
            return true;
        }

        @Override
        public void submit() {
            this.doWork();
        }

        @Override
        public void addDependentWorkflow(CosmosWorkflow workflow) {
            throw new UnsupportedOperationException("Not supported");
        }

        public boolean isCompleted() {
            return completed;
        }

        private void doWork() {
            try {
                Thread.sleep(50L);
            } catch (InterruptedException ignoreThis) {}
            this.completed = true;
        }
    }

    private FakeJob deps[];
    private PigScriptJob instance;

    @Before
    public void setUp() throws Exception {
        PigServer pigServer = mock(PigServer.class);

        this.deps = new FakeJob[] { new FakeJob(), new FakeJob() };

        this.instance = new PigScriptJob(pigServer,
                this.getClass().getResource("/empty-script.pig"), null);
    }

    @Test
    public void shouldAddDependencies() throws Exception {
        this.loadDependencies();
    }

    @Test
    public void shouldReturnWithNoBlockingOnSubmit() throws Exception {
        this.loadDependencies();
        final long t1 = System.currentTimeMillis();
        this.instance.submit();
        final long t2 = System.currentTimeMillis();
        assertTrue((t2 - t1) < 100L);
    }

    @Test
    public void shouldWaitForCompletion() throws Exception {
        this.loadDependencies();
        final long t1 = System.currentTimeMillis();
        this.instance.submit();
        this.instance.waitForCompletion(true);
        final long t2 = System.currentTimeMillis();
        assertTrue((t2 - t1) > 100L);
    }

    @Test
    public void shouldSubmitDependencies() throws Exception {
        this.loadDependencies();
        this.instance.submit();
        this.instance.waitForCompletion(true);
        for (FakeJob dep : this.deps) {
            assertTrue(dep.isCompleted());
        }
    }

    private void loadDependencies() {
        for (FakeJob dep : this.deps) {
            this.instance.addDependentWorkflow(dep);
        }
    }

}
