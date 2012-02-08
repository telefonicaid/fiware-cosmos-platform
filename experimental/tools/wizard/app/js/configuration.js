/**
 * @author DANIEL ESONO FERRER
 * @author JAVIER JIMENEZ GAZOL
 */

//Creation complete function
$().ready(function()
{
	//Hide second option of th input path field and forms number 2 and 3
	$('#file').hide();
	$('#secondForm').hide();
	$('#thirdForm').hide();
	
	//Init disable buttons
	document.getElementById("backButton").disabled = true;
	document.getElementById("finishButton").disabled = true;
	
	//Init selected first <a> 
	document.getElementById("aFirst").style.fontWeight = "bold";
	
		
		
	//Cancel function
   $("#cancelButton").click(function()
	{
     	location.href = "../../index.html";
   });
   
   //Next function
   $("#nextButton").click(function()
	{		
    	if(document.getElementById("backButton").disabled == true)
    	{
    		$('#firstForm').hide();
    		$('#secondForm').show("slow");
    		
    		document.getElementById("backButton").disabled = false;
    		
    		document.getElementById("aFirst").style.fontWeight = "normal";
    		document.getElementById("aSecond").style.fontWeight = "bold";
    	}
    	
    	else
    	{
    		$('#secondForm').hide();
    		$('#thirdForm').show("slow");
    		
    		document.getElementById("nextButton").disabled = true;
    		document.getElementById("finishButton").disabled = false;
    		
    		document.getElementById("aSecond").style.fontWeight = "normal";
    		document.getElementById("aThird").style.fontWeight = "bold";
    	}
   });
   
   //Back function
   $("#backButton").click(function()
	{		
    	if(document.getElementById("finishButton").disabled == true)
    	{
    		$('#secondForm').hide();
    		$('#firstForm').show("slow");
    		
    		document.getElementById("backButton").disabled = true;
    		
    		document.getElementById("aFirst").style.fontWeight = "bold";
    		document.getElementById("aSecond").style.fontWeight = "normal";
    	}
    	
    	else
    	{
    		$('#thirdForm').hide();
    		$('#secondForm').show("slow");
    		
    		document.getElementById("nextButton").disabled = false;
    		document.getElementById("finishButton").disabled = true;
    		
    		document.getElementById("aSecond").style.fontWeight = "bold";
    		document.getElementById("aThird").style.fontWeight = "normal";
    	}
   });
   
   //Finish function
   $("#finishButton").click(function()
	{
		alert("Your BDP Configuration has been finished successfully...!!!");
     	location.href = "../../index.html";
   });
   
   //Radio button 1 function
   $("#radio1").click(function()
	{
    	document.getElementById("path").innerHTML = "URL path";
    	$('#file').hide();
    	$('#url').show("slow");
   });
   
   //Radio button 2 function
   $("#radio2").click(function()
	{
    	document.getElementById("path").innerHTML = "File path";
    	$('#url').hide();
    	$('#file').show("slow");
   });
		
});
