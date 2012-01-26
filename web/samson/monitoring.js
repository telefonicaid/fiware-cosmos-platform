//**********************************************************************************
//LISTS
//**********************************************************************************
fetchDataList();  
function fetchDataList() {		
	$.ajax({
			 //url: "http://localhost/samson/graphs_parameters.json",
			 url: "get.php",
			 method: 'GET',
			 dataType: 'json',
			 success: onDataReceivedList
		  });						
	setTimeout('fetchDataList()', 10000);
						
	function onDataReceivedList(series) {				
		//SYSTEM
			var num_aux					= "";
			var comma					= "";
			var zeros					= "";
			var value_K					= 1000;
			var value_M					= value_K * 1000;
			var value_G					= value_M * 1000;
			var value_T					= value_G * 1000;
			var system_digit			= 0;
			var system_result           = 0;
			var system_units			= "";
			var system_header 	= "<table id='hor-zebra'>";
			var system_content 	= "";
			var system_current  = series.system.current;
			var datasize_p = document.params_info.datasize.id;
			var cores_p = document.params_info.cores.id;

			for(system_list in system_current){
					system_header 	= system_header+"<th scope='col'>"+system_list+"</th>";
					if((system_current[system_list]<value_K)){
						system_units = "";
						system_result = system_current[system_list];
					}
					if((system_current[system_list]>=value_K)&&(system_current[system_list]<value_M)){
						system_units = "";
						system_result = system_current[system_list]/value_K;
						system_units = "K";
					}
					if((system_current[system_list]>=value_M)&&(system_current[system_list]<value_G)){
						system_units = "";
						system_result = system_current[system_list]/value_M;
						system_units = "M";
					}
					if((system_current[system_list]>=value_G)&&(system_current[system_list]<value_T)){
						system_units = "";
						system_result = system_current[system_list]/value_G;
						system_units = "G";
					}
					if(system_current[system_list]>=value_T){
						system_units = "";
						system_result = system_current[system_list]/value_T;
						system_units = "T";
					}
					if(system_result<10){
						system_result = Math.round(system_result*100)/100;
					}else if(system_result<100){
						system_result = Math.round(system_result*10)/10;
					}else{
						system_result = Math.round(system_result);
					}
					system_current[system_list] = system_result+system_units;
					system_content 	= system_content+"<td>"+system_current[system_list]+"</td>";
			}
			document.getElementById("lists_system").innerHTML = "<thead><tr>"+system_header+"</tr></thead><tbody><tr>"+system_content+"</tr></tbody></table>";
			document.params_info.datasize.value = series.system.current[datasize_p]+"Gb";
			document.params_info.cores.value = series.system.current[cores_p];

		//QUEUES			
			var queues_header 	= "<table id='hor-zebra'><th scope='col'>name</th><th scope='col'>format</th>";
			var queues_content 	= "";	
			var total_queues 	= series.queues.length;
			var q_header 		= 0;
			var value_K					= 1000;
			var value_M					= value_K * 1000;
			var value_G					= value_M * 1000;
			var value_T					= value_G * 1000;
			var queues_result           = 0;
			var queues_units			= "";

			for(i=0;i<total_queues;i++){
				var num_aux					= "";
				var comma					= "";
				var zeros					= "";
				var queues_digit			= 0;
				var queues_name 			= series.queues[i].name;
				var queues_format   		= series.queues[i].format;
				var queues_current  		= series.queues[i].current;
					queues_content 			= queues_content+"<td>"+queues_name+"</td><td>"+queues_format+"</td>";
				for(queues_list in queues_current){
					if((queues_current[queues_list]<value_K)){
						queues_units = "";
						queues_result = queues_current[queues_list];
					}
					if((queues_current[queues_list]>=value_K)&&(queues_current[queues_list]<value_M)){
						queues_units = "";
						queues_result = queues_current[queues_list]/value_K;
						queues_units = "K";
					}
					if((queues_current[queues_list]>=value_M)&&(queues_current[queues_list]<value_G)){
						queues_units = "";
						queues_result = queues_current[queues_list]/value_M;
						queues_units = "M";
					}
					if((queues_current[queues_list]>=value_G)&&(queues_current[queues_list]<value_T)){
						queues_units = "";
						queues_result = queues_current[queues_list]/value_G;
						queues_units = "G";
					}
					if(queues_current[queues_list]>=value_T){
						queues_units = "";
						queues_result = queues_current[queues_list]/value_T;
						queues_units = "T";
					}
					if(queues_result<10){
						queues_result = Math.round(queues_result*100)/100;
					}else if(queues_result<100){
						queues_result = Math.round(queues_result*10)/10;
					}else{
						queues_result = Math.round(queues_result);
					}
					queues_current[queues_list] = queues_result+queues_units;

						if(q_header == 0){
							queues_header 	= queues_header+"<th scope='col'>"+queues_list+"</th>";
						}
						queues_content 	= queues_content+"<td>"+queues_current[queues_list]+"</td>";
				}
				q_header = 1;

				queues_content 	= queues_content+"</tr>";
									
			}

			document.getElementById("lists_queues").innerHTML = "<thead><tr>"+queues_header+"</tr></thead><tbody><tr>"+queues_content+"</tr></tbody></table>";
	}
}
//**********************************************************************************
//GRAPHICS
//**********************************************************************************
var init			= 0;
var typesystem		= 0;
var timesystem		= 0;
var typequeues		= 0;
var timequeues		= 0;
var queue_label		= 0;
var queue_param 	= 0;
var system_param   	= 0;
	
fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);
			
     function fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init) {
			$.ajax({
                //url: "http://localhost/samson/graphs_parameters.json",
			    url: "get.php",
                method: 'GET',
                dataType: 'json',
                success: onDataReceivedGraph
            });
			setTimeout('fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,1)', 10000);
			
			function onDataReceivedGraph(series) {
				//SYSTEM
					var plot_system_array_data 		= new Array();
					var system_p 					= new Array();
					var counter_system 				= 0;
					var value_param_system 			= 0;
					var system_history 				= series.system.history[timesystem];
					for(elems_system in system_history){
						if(init == 0){				
							document.params_system.innerHTML = document.params_system.innerHTML+"<li><input type='checkbox' id="+elems_system+" value='"+value_param_system+"' name='sys_param' onChange='selectParamSystem("+value_param_system+");' checked='checked'/><label for='"+elems_system+"'>"+elems_system+"</label></li>";
							value_param_system = value_param_system + 1;
						}
						counter_system = counter_system + 1;
					}
					
					if(counter_system != 0){

						if(typesystem == 0){
							var options = {
											series:{
													points:{
															show: true,
													},
													lines:{
															show: true,
													},
											},
							}
						}else{
							var options = {
											series:{
													bars:{
															show: true,
													},
											},
							}
						}
						
						var activecb_system = 0;
						var active_elem_system = "";

						if(counter_system == 1){
							
							activecb_system = activecb_system + 1;
							system_p = document.params_system.sys_param.id;
	
							if(activecb_system == 0){	
								var counter =	0;
								for(active_elem_system in system_history){
									counter++;
								}
								var i = 0;
								for(active_elem_system in system_history){
									original_system_data = series.system.history[timesystem][active_elem_system];
									plot_system_array_data = plot_system_array_data.concat(graph_system_data(original_system_data,active_elem_system,(i/counter))[active_elem_system]);
									i++;
								}
							}else{
								for(j=0;j<counter_system;j++){
									active_elem_system = system_p;
									original_system_data = series.system.history[timesystem][active_elem_system];
									plot_system_array_data = plot_system_array_data.concat(graph_system_data(original_system_data,active_elem_system,0)[active_elem_system]);
								}
							}
						}else{
							for(i=0;i<counter_system;i++){
								if(document.params_system.sys_param[i].checked){
									activecb_system = activecb_system + 1;
									system_p[i] = document.params_system.sys_param[i].id;
								}else{
									system_p[i] = "null";
								}
							}
							if(activecb_system == 0){	
								var counter =	0;
								for(active_elem_system in system_history){
									counter++;
								}
								var i = 0;
								for(active_elem_system in system_history){
									original_system_data = series.system.history[timesystem][active_elem_system];
									plot_system_array_data = plot_system_array_data.concat(graph_system_data(original_system_data,active_elem_system,(i/counter))[active_elem_system]);
									i++;
								}
							}else{
								for(j=0;j<counter_system;j++){
									if(system_p[j] != "null"){
										active_elem_system = system_p[j];
										original_system_data = series.system.history[timesystem][active_elem_system];
										plot_system_array_data = plot_system_array_data.concat(graph_system_data(original_system_data,active_elem_system,0)[active_elem_system]);
									}
								}
							}
						}
						document.getElementById("y_axis_system").innerHTML = "10<sup>y</sup>";
						$.plot($("#plot_system"), plot_system_array_data, options);	
					}
					
				//QUEUES
					
						var plot_queues_array_data 		= new Array();
						var queues_l 					= new Array();
						var queues_p 					= new Array();
						var counter_queues 				= 0;
						var counter_params_queues 		= 0;
						var value_label_queues 			= 0;
						var value_param_queues 			= 0;
						var elems_queues_label 			= "";
						var total_queues				= series.queues.length;

						for(i=0;i<total_queues;i++){
							var queues_history 				= series.queues[queue_label].history[timesystem];
							if(init == 0){
								elems_queues_label = series.queues[i].name;
								document.labels_queues.innerHTML = document.labels_queues.innerHTML+"<li><input type='checkbox' id="+elems_queues_label+" value='"+value_label_queues+"' name='q_label' onChange='selectLabelQueues("+value_label_queues+");' checked='checked'/><label for='"+elems_queues_label+"'>"+elems_queues_label+"</label></li>";	
								value_label_queues = value_label_queues + 1;
							}
							counter_queues = counter_queues + 1;
						}
						
						for(elems_queues in queues_history){
							if(init == 0){				
								document.params_queues.innerHTML = document.params_queues.innerHTML+"<li><input type='checkbox' id="+elems_queues+" value='"+value_param_queues+"' name='q_param' onChange='selectParamQueues("+value_param_queues+");' /><label for='"+elems_queues+"'>"+elems_queues+"</label></li>";						
								value_param_queues = value_param_queues + 1;
							}
							counter_params_queues = counter_params_queues + 1;
						}
						if(counter_params_queues != 0){
							if(typequeues == 0){
								var options = {
												series:{
														points:{
																show: true,
														},
														lines:{
																show: true,
														},
												},
								}
							}else{
								var options = {
												series:{
														bars:{
																show: true,
														},
												},
								}
							}
							
							var activecb_queues = 0;
							var active_queues = "";
							var active_elem_queues = "";
							var queue_or_param = "queue";
							
							for(i=0;i<total_queues;i++){
								if(document.labels_queues.q_label[i].checked){
									activecb_queues = activecb_queues + 1;
									queues_l[i] = document.labels_queues.q_label[i].id;
								}else{
									queues_l[i] = "null";
								}
							}
							
							if(counter_params_queues==1){
								if(init==0){
									document.params_queues.q_param.checked = true;
								}
								queues_p = document.params_queues.q_param.id;
		
								if((activecb_queues == 0)||(activecb_queues == total_queues)){	
									var active_queue_param = document.params_queues.q_param.id;
									for(j=0;j<total_queues;j++){
										active_elem_queues = document.labels_queues.q_label[j].id;
										active_queues = active_queue_param;
										original_queues_data = series.queues[j].history[timequeues][active_queue_param];
										plot_queues_array_data = plot_queues_array_data.concat(graph_queues_data(original_queues_data,active_elem_queues,queue_or_param,active_queues)[active_elem_queues]);
									}
								}else{
									if(activecb_queues == 1){
										queue_or_param = "param";
										active_elem_queues = queues_p;
										for(i=0;i<counter_queues;i++){
											if(document.labels_queues.q_label[i].checked){
												active_queues = document.labels_queues.q_label[i].id;
												original_queues_data = series.queues[i].history[timequeues][active_elem_queues];
												plot_queues_array_data = plot_queues_array_data.concat(graph_queues_data(original_queues_data,active_elem_queues,queue_or_param, active_queues)[active_elem_queues]);
											}
										}
									}else{
										var active_queue_param = document.params_queues.q_param.id;
										for(j=0;j<total_queues;j++){
											if(queues_l[j] != "null"){
												active_elem_queues = queues_l[j];
												active_queues = active_queue_param;
												original_queues_data = series.queues[j].history[timequeues][active_queue_param];
												plot_queues_array_data = plot_queues_array_data.concat(graph_queues_data(original_queues_data,active_elem_queues,queue_or_param,active_queues)[active_elem_queues]);
											}
										}
									}
								}
							}else{
								if (init == 0){
									document.params_queues.q_param[0].checked = true;	
								}
								
								for(j=0;j<counter_params_queues;j++){
									if(document.params_queues.q_param[j].checked){
										queues_p[j] = document.params_queues.q_param[j].id;
									}else{
										queues_p[j] = "null";
									}
								}
								
								
								if((activecb_queues == 0)||(activecb_queues == total_queues)){	
									var active_queue_param = document.params_queues.q_param[queue_param].id;
									for(j=0;j<total_queues;j++){
										active_elem_queues = document.labels_queues.q_label[j].id;
										active_queues = active_queue_param;
										original_queues_data = series.queues[j].history[timequeues][active_queue_param];
										plot_queues_array_data = plot_queues_array_data.concat(graph_queues_data(original_queues_data,active_elem_queues,queue_or_param,active_queues)[active_elem_queues]);
									}
								}else{
									if(activecb_queues == 1){
										queue_or_param = "param";
										for(j=0;j<counter_params_queues;j++){
											if(queues_p[j] != "null"){
												active_elem_queues = queues_p[j];
												for(i=0;i<counter_queues;i++){
													if(document.labels_queues.q_label[i].checked){
														active_queues = document.labels_queues.q_label[i].id;
														original_queues_data = series.queues[i].history[timequeues][active_elem_queues];
														plot_queues_array_data = plot_queues_array_data.concat(graph_queues_data(original_queues_data,active_elem_queues,queue_or_param, active_queues)[active_elem_queues]);
													}
												}
											}
										}
									}else{
										var active_queue_param = document.params_queues.q_param[queue_param].id;
										for(j=0;j<total_queues;j++){
											if(queues_l[j] != "null"){
												active_elem_queues = queues_l[j];
												active_queues = active_queue_param;
												original_queues_data = series.queues[j].history[timequeues][active_queue_param];
												plot_queues_array_data = plot_queues_array_data.concat(graph_queues_data(original_queues_data,active_elem_queues,queue_or_param,active_queues)[active_elem_queues]);
											}
										}
									}
								}
							}
							document.getElementById("y_axis_queues").innerHTML = "10<sup>y</sup>";
							$.plot($("#plot_queues"), plot_queues_array_data, options);	
						}
                	}
            	}

			function graph_system_data(original_system_data,active_elem,offset){

						var plot_system_data = new Object();
								plot_system_data[active_elem] 				= new Object();
									plot_system_data[active_elem].data  	= new Array();
									plot_system_data[active_elem].label 	= active_elem;

						for (k=0;k<original_system_data.length;k++){
								var axis_x = k -original_system_data.length+1+offset ;
								var axis_y;
								if( original_system_data[k] <=1 )
								    axis_y = 0;
								else
								    axis_y = Math.log(original_system_data[k])/Math.log(10);								    

								plot_system_data[active_elem].data[k] 		= [axis_x,axis_y];
						}

						return plot_system_data;

			}
			
			function graph_queues_data(original_queues_data,active_elem_queues,queue_or_param,active_queues){

				var plot_queues_data = new Object();
					plot_queues_data[active_elem_queues] 		= new Object();
					plot_queues_data[active_elem_queues].data  	= new Array();
							
					if(queue_or_param == "queue"){
						plot_queues_data[active_elem_queues].label 	= active_elem_queues+' - '+active_queues;
					}else{
						plot_queues_data[active_elem_queues].label 	= active_queues+' - '+active_elem_queues;
					}

					for (k=0;k<=original_queues_data.length;k++){
							var axis_x = k -original_queues_data.length+1 ;
							
							var axis_y;

							if( original_queues_data[k] <= 1)
							    axis_y = 0;
							else
							    axis_y = Math.log(original_queues_data[k])/Math.log(10);

							plot_queues_data[active_elem_queues].data[k] 		= [axis_x,axis_y];
					}

					return plot_queues_data;
			}
			//Select Type on System
			function selectTypeSystem(value_type_system){
				var init = 1;
				typesystem = value_type_system;		
				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);	
			}
			//Select Time on System
			function selectTimeSystem(value_time_system){
				var init = 1;
				timesystem = value_time_system;				
				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);				
			}
			//Select Parameter on System
			function selectParamSystem(value_param_system) {
				var init = 1;
				var total_params_system = document.params_system.length;
				if(total_params_system==1){
					if(document.params_system.sys_param.checked){
						system_param 	= document.params_system.sys_param.value;
					}
				}else{
					if(document.params_system.sys_param[value_param_system].checked){
							system_param 	= document.params_system.sys_param[value_param_system].value;
					}
				}

				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);
			}
			//Select Type on Queues
			function selectTypeQueues(value_type_queues){
				var init = 1;
				typequeues = value_type_queues;		
				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);	
			}
			//Select Time on Queues
			function selectTimeQueues(value_time_queues){
				var init = 1;
				timequeues = value_time_queues;		
				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);	
			}
			//Select Queue
			function selectLabelQueues(value_label_queues) {
				var init = 1;
				var num_labels_queues_checked = 0;
				var num_params_queues_checked = 0;
				var total_labels_queues = document.labels_queues.length;
				var total_params_queues = document.params_queues.length;
				
				if(total_params_queues != 1){
					for(i=0;i<total_labels_queues;i++){
						if(document.labels_queues.q_label[i].checked){
							num_labels_queues_checked = num_labels_queues_checked + 1;
						}
					}
					for(j=0;j<total_params_queues;j++){
						if(document.params_queues.q_param[j].checked){
							num_params_queues_checked = num_params_queues_checked + 1;
							var value_param_queues = document.params_queues.q_param[j].value;
						}
						if(num_params_queues_checked == total_params_queues){
							value_param_queues = 0;
						}
					}
					if(num_labels_queues_checked != 1){
						for(k=0;k<total_params_queues;k++){
							document.params_queues.q_param[k].checked = false;
						}
						document.params_queues.q_param[value_param_queues].checked = true;
					}
					if(document.labels_queues.q_label[value_label_queues].checked){
							queue_label 	= document.labels_queues.q_label[value_label_queues].value;
					}
				}
				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);
			}
			//Select Parameter on Queues
			function selectParamQueues(value_param_queues) {
				var init = 1;
				var num_labels_queues_checked = 0;
				var total_labels_queues = document.labels_queues.length;
				var total_params_queues = document.params_queues.length;
				
				for(i=0;i<total_labels_queues;i++){
					if(document.labels_queues.q_label[i].checked){
						num_labels_queues_checked = num_labels_queues_checked + 1;
					}
				}
				if(num_labels_queues_checked != 1){
					if(total_params_queues == 1){
						if(document.params_queues.q_param.checked){
							queue_param 	= document.params_queues.q_param.value;
						}
					}else{
						for(i=0;i<total_params_queues;i++){
							if(i != value_param_queues){
								document.params_queues.q_param[i].checked = false;
							}
						}
						if(document.params_queues.q_param[value_param_queues].checked){
								queue_param 	= document.params_queues.q_param[value_param_queues].value;
						}
					}
				}

				fetchDataGraph(typesystem,timesystem,typequeues,timequeues,system_param,queue_label,queue_param,init);
			}
