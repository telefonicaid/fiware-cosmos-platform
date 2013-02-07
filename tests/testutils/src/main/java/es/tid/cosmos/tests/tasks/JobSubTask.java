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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.thrift.TException;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransportException;

import es.tid.cosmos.tests.environment.Environment;
import es.tid.cosmos.tests.environment.EnvironmentSetting;
import es.tid.cosmos.tests.jobsub.om.generated.*;

import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNull;

/**
 *
 * @author ximo
 */
public class JobSubTask extends Task {
    private final Environment env;
    private final TSocket transport;
    private SubmissionPlan plan;
    private SubmissionHandle handle;
    private final JobSubmissionService.Client service;
    private final String[] attributes;

    public JobSubTask(Environment env, SubmissionPlan plan, String[] attributes)
            throws TTransportException {
        this.transport = new TSocket(
                env.getProperty(EnvironmentSetting.FrontendServer),
                Integer.parseInt(env.getProperty(
                        EnvironmentSetting.FrontendThriftPort)));
        this.transport.open();
        TBinaryProtocol protocol = new TBinaryProtocol(this.transport);

        this.service = new JobSubmissionService.Client(protocol);
        this.plan = plan;
        this.env = env;
        this.attributes = attributes;
    }

    public void close() {
        this.transport.close();
    }

    @Override
    public void run() throws TestException {
        assertNull(this.handle, "Verifying handle is null");
        try {
            this.handle = this.service.submit(this.plan);
        } catch (SubmissionError ex) {
            throw new TestException("Failed job submission", ex);
        } catch (TException ex) {
            throw new TestException("Failed job submission", ex);
        }
    }

    @Override
    public TaskStatus getStatus() throws TestException {
        if (this.handle == null) {
            return TaskStatus.Created;
        }

        JobData jobData;
        try {
            jobData = this.service.get_job_data(this.handle);
        } catch (SubmissionError ex) {
            throw new TestException("Failed job submission", ex);
        } catch (TException ex) {
            throw new TestException("Failed job submission", ex);
        }
        return JobSubTask.getStatusFromJobsubStatus(jobData.getState());
    }

    private static TaskStatus getStatusFromJobsubStatus(State state)
            throws TestException {
        switch (state) {
            case SUCCESS:
                return TaskStatus.Completed;
            case SUBMITTED: case RUNNING:
                return TaskStatus.Running;
            case FAILURE:
            case ERROR:
                return TaskStatus.Error;
            default:
                throw new TestException("Unknown state: " + state);
        }
    }

    @Override
    public List<Map<String, String>> getResults() throws TestException {
        FileSystem fs;
        try {
            assertEquals(this.getStatus(), TaskStatus.Completed,
                         "Verifying task is completed");
            final String frontendName = this.env.getProperty(
                    EnvironmentSetting.FrontendServer);
            fs = FileSystem.get(
                    new URI("hdfs://" + frontendName), new Configuration());
        } catch (URISyntaxException ex) {
            throw new TestException("Bad URI", ex);
        } catch (IOException ex) {
            throw new TestException("[IOException]", ex);
        }

        List<Map<String, String>> ret = new LinkedList<Map<String, String>>();
        BufferedReader output = null;
        try {
            output = new BufferedReader(new InputStreamReader(fs.open(null)));

            String line;
            while ((line = output.readLine()) != null) {
                String[] tokens = line.split("\\s");
                Map<String, String> lineVal = new HashMap<String, String>();
                assertEquals(tokens.length, this.attributes.length,
                             "Verifying line has the same amout of tokens as"
                        + " attributes passed in the constructor");
                for (int i = 0; i < this.attributes.length; i++) {
                    lineVal.put(this.attributes[i], tokens[i]);
                }
                ret.add(lineVal);
            }
        } catch (IOException ex) {
            throw new TestException("[IOException]", ex);
        } finally {
            if (output != null) {
                try {
                    output.close();
                } catch (IOException ex) {
                    throw new TestException("[IOException]", ex);
                }
            }
        }

        return ret;
    }
}
