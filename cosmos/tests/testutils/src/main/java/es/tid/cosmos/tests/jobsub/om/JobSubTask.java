package es.tid.cosmos.tests.jobsub.om;

import java.util.List;
import java.util.Map;

import org.apache.thrift.TException;
import org.apache.thrift.protocol.TBinaryProtocol;
import org.apache.thrift.transport.TSocket;
import org.apache.thrift.transport.TTransportException;
import static org.testng.Assert.assertNull;

import es.tid.cosmos.tests.jobsub.om.generated.*;
import es.tid.cosmos.tests.tasks.*;

/**
 *
 * @author ximo
 */
public class JobSubTask extends Task {
    private SubmissionPlan plan;
    private SubmissionHandle handle;
    private JobSubmissionService.Client service;

    public JobSubTask(Environment env, SubmissionPlan plan)
            throws TTransportException {
        TSocket transport = new TSocket(
                env.getProperty(EnvironmentSetting.FRONTEND_SERVER),
                Integer.parseInt(env.getProperty(EnvironmentSetting.FRONTEND_THRIFT_PORT)));
        transport.open();
        TBinaryProtocol protocol = new TBinaryProtocol(transport);

        this.service = new JobSubmissionService.Client(protocol);
        this.plan = plan;
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
            case SUBMITTED:
            case RUNNING:
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
        throw new UnsupportedOperationException("Not supported yet.");
    }
}
