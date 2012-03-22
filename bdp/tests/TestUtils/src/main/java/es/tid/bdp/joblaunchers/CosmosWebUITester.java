package es.tid.bdp.joblaunchers;

import es.tip.bdp.frontend_om.FrontEnd;
import es.tip.bdp.frontend_om.SelectInputPage;
import es.tip.bdp.frontend_om.SelectJarPage;
import es.tip.bdp.frontend_om.SelectNamePage;
import java.io.File;
import java.util.Map;
import java.util.UUID;
import org.testng.Assert;

public class CosmosWebUITester implements JobLauncher
{
    private FrontEnd _frontend;
    
    public CosmosWebUITester() {
        _frontend = new FrontEnd();
    }
    
    public String createNewTask(String inputFilePath, String jarPath)
            throws TestException {        
        return createNewTask(inputFilePath, jarPath);
    }
    
    public String createNewTask(String inputFilePath,
                                String jarPath,
                                boolean launch)
            throws TestException {        
        // Verify input params
        if(!new File(inputFilePath).exists()) {
            throw new TestException("Input path does not exist");
        }
        
        if(!new File(jarPath).exists()) {
            throw new TestException("JAR path does not exist.");
        }
        SelectNamePage namePage = _frontend.goToCreateNewJob();
        final String taskId = UUID.randomUUID().toString();
        namePage.setName(taskId);
        
        SelectJarPage jarPage = namePage.submitNameForm();
        
        jarPage.setInputJar(jarPath);
        SelectInputPage inputPage = jarPage.submitJarFileForm();
        
        inputPage.setInputFile(inputFilePath);
        inputPage.submitInputFileForm();        
              
        if(launch) {
            launchTask(taskId);
        }
        
        return taskId;
    }
    
    public void launchTask(String taskId) throws TestException {
        Assert.assertEquals(getTaskStatus(taskId),
                            TaskStatus.Created,
                            "Verifying task is in Created state");
        _frontend.launchJob(taskId);
    }

    public void waitForTaskCompletion(String taskId) throws TestException {
        boolean taskCompleted = false;
        
        while(!taskCompleted) {
            // Go to the main page
            _frontend.goHome();

            // Get task table

            // Get status of task with id==taskId
            
            // Check if task completed
        }
        
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public Map<String,String[]> getResults(String taskId) throws TestException {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public TaskStatus getTaskStatus(String taskId) throws TestException {
        throw new UnsupportedOperationException("Not supported yet.");
    }
}
