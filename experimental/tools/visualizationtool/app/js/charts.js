// Get URL parameters
$(document).ready(function() 
{
	$.urlParam = function(name)
	{
	    var results = new RegExp('[\\?&]' + name + '=([^&#]*)').exec(window.location.href);
	
		if( results )
		{
		    return results[1] || 0;
		}
		return 0;
	}
	// example.com?param1=name&param2=&id=6
	var chart = $.urlParam('chart'); 
	
	if( chart != 0 )
	{
		$.get("../../php/getLastChart.php", getLastChartHandler )
		// .success(function() { alert("second success"); })
  		// .error(function() { alert("error"); })
  		// .complete(function() { alert("complete"); });
	}
	else
	{
		createColumnChart();
	}
});

// Get Last Chart Created
function getLastChartHandler(chartData)
{
	var json = $.parseJSON(chartData);
	xmlChart($("#container").attr('id'),json.row[1],json.row[2],json.row[3],json.row[4],json.row[5]);
}

function createAreaChart()
{
	var chart;
	$(document).ready(function() {
		chart = new Highcharts.Chart({
			chart: {
				renderTo: 'container', 
			defaultSeriesType: 'area'
		},
		title: {
			text: 'BARCA/MADRID 2011 RESULTS'
		},
		xAxis: {
			labels: {
				formatter: function() {
					return this.value; // clean, unformatted number for year
				}
			}							
		},
		yAxis: {
			title: {
				text: 'Derby matches stats'
			},
			labels: {
				formatter: function() {
					return this.value / 1000 +'k';
				}
			}
		},
		tooltip: {
			formatter: function() {
				return this.series.name +' produced <b>'+
					Highcharts.numberFormat(this.y, 0) +'</b><br/>warheads in '+ this.x;
			}
		},
		plotOptions: {
			area: {
				pointStart: 1940,
				marker: {
					enabled: false,
					symbol: 'circle',
					radius: 2,
					states: {
						hover: {
							enabled: true
						}
					}
				}
			}
		},
		series: [{
			name: 'FC Barcelona',
			data: [null, null, null, null, null, 6 , 11, 32, 110, 235, 369, 640, 
				1005, 1436, 2063, 3057, 4618, 6444, 9822, 15468, 20434, 24126, 
				27387, 29459, 31056, 31982, 32040, 31233, 29224, 27342, 26662, 
				26956, 27912, 28999, 28965, 27826, 25579, 25722, 24826, 24605, 
				24304, 23464, 23708, 24099, 24357, 24237, 24401, 24344, 23586, 
				22380, 21004, 17287, 14747, 13076, 12555, 12144, 11009, 10950, 
				10871, 10824, 10577, 10527, 10475, 10421, 10358, 10295, 10104 ]
		}, {
			name: 'Real Madrid',
			data: [null, null, null, null, null, null, null , null , null ,null, 
			5, 25, 50, 120, 150, 200, 426, 660, 869, 1060, 1605, 2471, 3322, 
			4238, 5221, 6129, 7089, 8339, 9399, 10538, 11643, 13092, 14478, 
			15915, 17385, 19055, 21205, 23044, 25393, 27935, 30062, 32049, 
			33952, 35804, 37431, 39197, 45000, 43000, 41000, 39000, 37000, 
			35000, 33000, 31000, 29000, 27000, 25000, 24000, 23000, 22000, 
			21000, 20000, 19000, 18000, 18000, 17000, 16000]
			}]
		});	
	});
}

function createColumnChart()
{
	$(document).ready(function() {
		var chart = new Highcharts.Chart({
			chart: {
				renderTo: 'container',
				defaultSeriesType: 'column'
			},
			title: {
				text: 'Monthly Average Rainfall'
			},
			subtitle: {
				text: 'Source: WorldClimate.com'
			},
			xAxis: {
				categories: [
					'Jan', 
					'Feb', 
					'Mar', 
					'Apr', 
					'May', 
					'Jun', 
					'Jul', 
					'Aug', 
					'Sep', 
					'Oct', 
					'Nov', 
					'Dec'
				]
			},
			yAxis: {
				min: 0,
				title: {
					text: 'Rainfall (mm)'
				}
			},
			legend: {
				layout: 'vertical',
				backgroundColor: '#FFFFFF',
				align: 'left',
				verticalAlign: 'top',
				x: 100,
				y: 70,
				floating: true,
				shadow: true
			},
			tooltip: {
				formatter: function() {
					return ''+
						this.x +': '+ this.y +' mm';
				}
			},
			plotOptions: {
				column: {
					pointPadding: 0.2,
					borderWidth: 0
				}
			},
		        series: [{
				name: 'Tokyo',
				data: [49.9, 71.5, 106.4, 129.2, 144.0, 176.0, 135.6, 148.5, 216.4, 194.1, 95.6, 54.4]
		
			}, {
				name: 'New York',
				data: [83.6, 78.8, 98.5, 93.4, 106.0, 84.5, 105.0, 104.3, 91.2, 83.5, 106.6, 92.3]
		
			}, {
				name: 'London',
				data: [48.9, 38.8, 39.3, 41.4, 47.0, 48.3, 59.0, 59.6, 52.4, 65.2, 59.3, 51.2]
		
			}, {
				name: 'Berlin',
				data: [42.4, 33.2, 34.5, 39.7, 52.6, 75.5, 57.4, 60.4, 47.6, 39.1, 46.8, 51.1]
		
			}]
		});
	});
}

function createPieChart()
{
	$(document).ready(function() {
		chart = new Highcharts.Chart({
			chart: {
				renderTo: 'container',
				plotBackgroundColor: null,
				plotBorderWidth: null,
				plotShadow: false
			},
			title: {
				text: 'Browser market shares at a specific website, 2010'
			},
			tooltip: {
				formatter: function() {
					return '<b>'+ this.point.name +'</b>: '+ this.percentage +' %';
				}
			},
			plotOptions: {
				pie: {
					allowPointSelect: true,
					cursor: 'pointer',
					dataLabels: {
						enabled: true,
						color: '#000000',
						connectorColor: '#000000',
						formatter: function() {
							return '<b>'+ this.point.name +'</b>: '+ this.percentage +' %';
						}
					}
				}
			},
		    series: [{
				type: 'pie',
				name: 'Browser share',
				data: [
					['Firefox',   45.0],
					['IE',       26.8],
					{
						name: 'Chrome',    
						y: 12.8,
						sliced: true,
						selected: true
					},
					['Safari',    8.5],
					['Opera',     6.2],
					['Others',   0.7]
				]
			}]
		});
	});
}

function createLineChart()
{
	var chart;
	$(document).ready(function() {
		chart = new Highcharts.Chart({
			chart: {
				renderTo: 'container',
				defaultSeriesType: 'line',
				marginRight: 130,
				marginBottom: 25
			},
			title: {
				text: 'Monthly Average Temperature',
				x: -20 //center
			},
			subtitle: {
				text: 'Source: WorldClimate.com',
				x: -20
			},
			xAxis: {
				categories: ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 
					'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec']
			},
			yAxis: {
				title: {
					text: 'Temperature (°C)'
				},
				plotLines: [{
					value: 0,
					width: 1,
					color: '#808080'
				}]
			},
			tooltip: {
				formatter: function() {
		                return '<b>'+ this.series.name +'</b><br/>'+
						this.x +': '+ this.y +'°C';
				}
			},
			legend: {
				layout: 'vertical',
				align: 'right',
				verticalAlign: 'top',
				x: -10,
				y: 100,
				borderWidth: 0
			},
			series: [{
				name: 'Tokyo',
				data: [7.0, 6.9, 9.5, 14.5, 18.2, 21.5, 25.2, 26.5, 23.3, 18.3, 13.9, 9.6]
			}, {
				name: 'New York',
				data: [-0.2, 0.8, 5.7, 11.3, 17.0, 22.0, 24.8, 24.1, 20.1, 14.1, 8.6, 2.5]
			}, {
				name: 'Berlin',
				data: [-0.9, 0.6, 3.5, 8.4, 13.5, 17.0, 18.6, 17.9, 14.3, 9.0, 3.9, 1.0]
			}, {
				name: 'London',
				data: [3.9, 4.2, 5.7, 8.5, 11.9, 15.2, 17.0, 16.6, 14.2, 10.3, 6.6, 4.8]
			}]
		});
	});
}

function createBarChart()
{
	var chart;
	$(document).ready(function() {
		chart = new Highcharts.Chart({
			chart: {
				renderTo: 'container',
				defaultSeriesType: 'bar'
			},
			title: {
				text: 'Historic World Population by Region'
			},
			subtitle: {
				text: 'Source: Wikipedia.org'
			},
			xAxis: {
				categories: ['Africa', 'America', 'Asia', 'Europe', 'Oceania'],
				title: {
					text: null
				}
			},
			yAxis: {
				min: 0,
				title: {
					text: 'Population (millions)',
					align: 'high'
				}
			},
			tooltip: {
				formatter: function() {
					return ''+
						 this.series.name +': '+ this.y +' millions';
				}
			},
			plotOptions: {
				bar: {
					dataLabels: {
						enabled: true
					}
				}
			},
			legend: {
				layout: 'vertical',
				align: 'right',
				verticalAlign: 'top',
				x: -100,
				y: 100,
				floating: true,
				borderWidth: 1,
				backgroundColor: '#FFFFFF',
				shadow: true
			},
			credits: {
				enabled: false
			},
		        series: [{
				name: 'Year 1800',
				data: [107, 31, 635, 203, 2]
			}, {
				name: 'Year 1900',
				data: [133, 156, 947, 408, 6]
			}, {
				name: 'Year 2008',
				data: [973, 914, 4054, 732, 34]
			}]
		});	
	});
}