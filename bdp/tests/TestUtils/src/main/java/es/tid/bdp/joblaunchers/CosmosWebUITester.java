package es.tid.bdp.joblaunchers;

import java.io.File;
import java.util.Map;
import java.util.UUID;

import org.testng.Assert;

import es.tip.bdp.frontend_om.FrontEnd;
import es.tip.bdp.frontend_om.SelectInputPage;
import es.tip.bdp.frontend_om.SelectJarPage;
import es.tip.bdp.frontend_om.SelectNamePage;

/**
 *
 * @author ximo
 */
public class CosmosWebUITester implements JobLauncher {
    private FrontEnd frontend;
    
    public CosmosWebUITester() {
        this.frontend = new FrontEnd();
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
        
        // Set name
        SelectNamePage namePage = this.frontend.goToCreateNewJob();
        final String taskId = UUID.randomUUID().toString();
        namePage.setName(taskId);
        
        // Set snd submit JAR
        SelectJarPage jarPage = namePage.submitNameForm();        
        jarPage.setInputJar(jarPath);
        SelectInputPage inputPage = jarPage.submitJarFileForm();
        
        // Set and submit input file
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
        this.frontend.launchJob(taskId);
    }

    public void waitForTaskCompletion(String taskId) throws TestException {
        boolean taskCompleted = false;
        
        while(!taskCompleted) {
            // Go to the main page
            this.frontend.goHome();
            taskCompleted = (this.frontend.getTaskStatus(taskId)
                                == TaskStatus.Completed);
            if(!taskCompleted) {
                try {
                    Thread.sleep(30000); // Sleep 30 seconds
                } catch (InterruptedException ex) {
                    throw new TestException("[InterruptedException] "
                            + ex.getMessage());
                }
            }
        }
    }
    
    public Map<String,String[]> getResults(String taskId) throws TestException {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public TaskStatus getTaskStatus(String taskId) throws TestException {
        return this.frontend.getTaskStatus(taskId);
    }
}
