/**
 * @author DANIEL ESONO FERRER
 * @author JAVIER JIMENEZ GAZOL
 */

//Creation complete function: set the events to validate the fields
$(document).ready(function()
{ 			
	var uploadbtn = document.getElementById("uploadbtn"); 
	var cleanbtn = document.getElementById("cleanbtn"); 
	
	// var i1 = document.getElementById("name").onkeyup( formValidation ); 
	// var i2 = document.getElementById("tit").onkeyup( formValidation ); 
	// var i3 = document.getElementById("dat").onkeyup( formValidation ); 	
	
	var t1ck=false;		
	document.getElementById("uploadbtn").disabled = false;			
	
	// Crash with jquery
	// i1.onkeyup = formValidation; 
	// i2.onkeyup = formValidation; 
	// i3.onkeyup = formValidation; 
	// cleanbtn.onclick = resetForm;	
	// resetForm();	
	
	hideForm();
	//Capture Combobox Changes
	$("#combo").change( comboHandler );
});

//Function to validate all the fields of the Creation form
function formValidation(oEvent)
{ 		
	oEvent = oEvent || window.event; 		
	var txtField = oEvent.target || oEvent.srcElement; 			
	
	var t1ck=true;			
	var msg=" ";			
	if(document.getElementById("name").value.length < 1){ t1ck=false; msg = msg + "Your user name should be minimun 1 char length";}			
	if(document.getElementById("tit").value.length < 1){ t1ck=false; msg = msg + "Your title should be minimun 1 char length";}		
	if(document.getElementById("dat").value.length < 1){ t1ck=false; msg = msg + "Your info data should be minimun 1 char length";}	
	
	if(t1ck){document.getElementById("uploadbtn").disabled = false; }			
	else{document.getElementById("uploadbtn").disabled = true; }		
} 

//Function to save
function clickSubmit()
{
    // fetch the data for the form
    var formData = $('#formComponent').serializeArray();
    
    $.post("../php/uploadChart.php", formData, clickSubmitHandler)
}

// Handler when form has been posted correctly
function clickSubmitHandler(data)
{
	 var url; 	
	 var chart = parseInt($("#combo option:selected").val() );
	 switch( chart )
	 {
		case 1:
			url = "../../app/view/charts/area.html";
			break;
		case 2:
			url = "../../app/view/charts/bar.html";
			break;
		case 3:
			url = "../../app/view/charts/column.html";
			break;
		case 4:
			url = "../../app/view/charts/line.html";
			break;
		case 5:
			url = "../../app/view/charts/pie.html";
			break;
		default:
			alert('No Correct Chart Selection');
			break;
	 }
	
	 url = url + "?chart=" + chart;
	
	 if (alert('Your visualization has been created successfully!.'))
	 location.href = url;	
	 else location.href = url;
}

//Clean all the field of the form
function resetForm()
{		
	document.getElementById("uploadbtn").disabled = true; 		
	var frmMain = document.forms[0]; 		
	frmMain.reset();		
}

// Hide Input Params
function hideForm()
{
	 // Set default combo value
	 $("#combo").val(0);
	 
	 $('#xAxisTitleTd').hide();
	 $('#xAxisInputTd').hide();
	 $('#yAxisTitleTd').hide();
     $('#yAxisInputTd').hide();
     $('#dataTitleTd').hide();
	 $('#dataInputTd').hide();
	 $('#descTitleTd').hide();
	 $('#descInputTd').hide();
	 $('#submitTitleTd').hide();
	 $('#submitInputTd').hide();
	 
	 $('#chartContainer').hide();
}

//Capture combobox Changes Handler
function comboHandler()
{
     // Show after combo
     if( $("#combo").val() == 0 )
     {
     	hideForm();
     }
     else
     {
		 $('#xAxisTitleTd').show("slow");
		 $('#xAxisInputTd').show("slow");
		 $('#yAxisTitleTd').show("slow");
	     $('#yAxisInputTd').show("slow");
	     $('#dataTitleTd').show("slow");
		 $('#dataInputTd').show("slow");
		 $('#descTitleTd').show("slow");
		 $('#descInputTd').show("slow");
		 $('#submitTitleTd').show("slow");
		 $('#submitInputTd').show("slow");
     }
}