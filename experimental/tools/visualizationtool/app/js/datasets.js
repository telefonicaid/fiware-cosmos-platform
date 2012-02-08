﻿/**
 * @author DANIEL ESONO FERRER
 * @author JAVIER JIMENEZ GAZOL
 */

//Creation complete function
$(document).ready(function()
{ 
	$.get("../../app/php/datasets.php", getTableInfo);
	
	$("img")	
   		.mouseenter(function(evento)
   		{
   			//Mouse over function for the delete button
   			if( $(this).attr("src") == "../../resources/images/16x16/delete_off.png" )
			{
				var newSrc = $(this).attr("src").replace("../../resources/images/16x16/delete_off.png", "../../resources/images/16x16/delete_on.png");
				$(this).attr("src", newSrc);
			} 	
			
			//Mouse over function for the visualize button
			else if( $(this).attr("src") == "../../resources/images/16x16/right.png" )
			{
				var newSrc = $(this).attr("src").replace("../../resources/images/16x16/right.png", "../../resources/images/16x16/right_big.png");
				$(this).attr("src", newSrc);
			}							  					
   		})
   		
   		//Mouse out function, change the icon when mouse out.
   		.mouseleave(function(evento)
   		{
   			//Mouse out function for the delete button.
   			if( $(this).attr("src") == "../../resources/images/16x16/delete_on.png" )
			{
				var newSrc = $(this).attr("src").replace("../../resources/images/16x16/delete_on.png", "../../resources/images/16x16/delete_off.png");
  				$(this).attr("src", newSrc); 
			}
			
			//Mouse out function for the visualize button.
			else if( $(this).attr("src") == "../../resources/images/16x16/right_big.png" )
			{
				var newSrc = $(this).attr("src").replace("../../resources/images/16x16/right_big.png", "../../resources/images/16x16/right.png");
				$(this).attr("src", newSrc);
			}
   		});
});

//Function to load the selected chart
function loadChartView(type)
{
	var url = "../../app/view/charts/" + type.replace("btn","") + ".html";
	location.href = url;	
}

//Function to remove the selected row and its correspondient chart, of course
function removeRowChart(type)
{
	alert("You can´t remove charts loaded by de administrator");
}

function getTableInfo(data)
{
	var json = $.parseJSON(data);
	$.each(json, function (index, chart)
	{
		addToTable(chart.title, chart.type, chart.username, chart.date);
	});
}

function addToTable(title ,type, username, date)
{
	var tabla = document.getElementById("chartTable").getElementsByTagName("TBODY")[0];
	var tr = document.createElement("TR");
	tabla.appendChild(tr);
	
	var tlin = document.getElementById("chartTable").rows.length;
	var newRow = document.getElementById("chartTable").rows[tlin-1];
	
	var tdTitle = document.createElement("TD");
	tdTitle.appendChild(document.createTextNode(title));
	var tdType = document.createElement("TD");
	tdType.appendChild(document.createTextNode(type));
	var tdName = document.createElement("TD");
	tdName.appendChild(document.createTextNode(username));
	var tdDate = document.createElement("TD");
	tdDate.appendChild(document.createTextNode(date));
	var tdVis = document.createElement("TD");
/*	var img = new Image();
	img.src = "../../resources/images/16x16/right_big.png";
	tdVis.appendChild(document.createTextNode(img));*/
	var tdDelete = document.createElement("TD");
	
	newRow.appendChild(tdTitle);
	newRow.appendChild(tdType);
	newRow.appendChild(tdName);
	newRow.appendChild(tdDate);
	newRow.appendChild(tdVis);
	newRow.appendChild(tdDelete);
}