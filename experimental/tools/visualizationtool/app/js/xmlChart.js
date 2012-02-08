// Create Chart from XML
function xmlChart(chartContainer, title,type,xAxisTitle,yAxisTitle,xmlData)
{
	$(document).ready(function() {
			
		var options = {
			chart: {
				renderTo: chartContainer,
				type: 'column'
			},
			title: {
				text: title
			},
			xAxis: {
				categories: []
			},
			yAxis: {
				title: {
					text: yAxisTitle
				}
			},
			series: []
		};
		
		// Load the data from the XML file
		//'resources/data/recommended.xml' 
		//$.get(xmlTitle, function(xml) {
			
			var xmlDoc = $.parseXML( xmlData );
			
			// Split the lines
			var $xml = $(xmlDoc);
			
			// create series
			var seriesOptions = {					
					data: []
				};
			// push categories
			$xml.find('Recomendadas app').each(function(i, app) {
				options.xAxis.categories.push($(app).attr('name'));
				seriesOptions.data.push( parseInt( $(app).attr('veces') ) );
			});
			
			// add it to the options
			options.series.push(seriesOptions);
	
			var chart = new Highcharts.Chart(options);
		//});
	});
}

// Get Title Example
function getTitleExample()
{
	return "Example Title";
}

// Get xAxis Example
function getxAxisExample()
{
	return "name";
}

// Get yAxis Example
function getyAxisExample()
{
	return "veces";
}

// Get XML Example
function getXmlExample()
{
	var xml = "<?xml version='1.0'?> <Recomendadas> <app name='Mastermind' veces='99564'/> <app name='Solitary' veces='89635'/> <app name='Sims Bowling' veces='86900'/>";
	xml = xml + "<app name='Pedrosa GP 2007' veces='82502'/> <app name='Bubbles' veces='82448'/> <app name='Medal of Honor Airborne' veces='76914'/> <app name='Pang Returns' veces='66504'/>";
	xml = xml + "<app name='Ice Age 2: The Meltdown' veces='63746'/>";
	xml = xml + "<app name='Tomb Raider Legend' veces='57004'/>";
	xml = xml + "<app name='Star Wars: The Battle for the Republic' veces='56433'/>";
	xml = xml + "<app name='Battle Chess' veces='51852'/>";
	xml = xml + "<app name='FIFA Street 2' veces='51059'/>";
	xml = xml + "<app name='Powerpuff Girls Snowboarding' veces='47564'/>";
	xml = xml + "<app name='OC TV Series' veces='40503'/>";
	xml = xml + "<app name='English Trainer' veces='33059'/>";
	xml = xml + "<app name='Nani Roma 4x4 Raid' veces='23372'/>";
	xml = xml + "<app name='Hangman' veces='15092'/>";
	xml = xml + "<app name='VRally 3D' veces='14267'/>";
	xml = xml + "<app name='Bruce Lee' veces='9427'/>";
	xml = xml + "<app name='Star Wars: Battle above Coruscant' veces='7064'/>";
	xml = xml + "</Recomendadas>";
	return xml;
}
