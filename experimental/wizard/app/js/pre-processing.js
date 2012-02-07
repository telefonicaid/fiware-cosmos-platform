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
   
   //Next function
   $("#nextButton").click(function()
	{
     	location.href = "../../app/view/consumption.html";
   });
   
   //Back function
   $("#backButton").click(function()
	{
     	location.href = "../../app/view/ingestion.html";
   });
		
});