/**
 * @author DANIEL ESONO FERRER
 * @author JAVIER JIMENEZ GAZOL
 */

//Creation complete function
$().ready(function()
{
	//Cancel function
   $("#cancelButton").click(function()
	{
     	location.href = "../../index.html";
   });
   
   //Finish function
   $("#finishButton").click(function()
	{
		alert("Your BDP Configuration has been finished successfully...!!!");
     	location.href = "../../index.html";
   });
   
   //Back function
   $("#backButton").click(function()
	{
     	location.href = "../../app/view/pre-processing.html";
   });
		
});