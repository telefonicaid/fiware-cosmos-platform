#include "TEST_Module.h"

// Calculate variables for a month //
/////////////////////////////////////
void MACRO_mobmx_MobVar_calcvariables::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	//Inputs
	MRData_TelMonth telMonth;
	MRData_CellMx cellInfo;
			
	//Outputs
	MRData_UInt phone;
	MRData_MobVars mobVars;

	// Temp
	MRData_TempMob tempMob;
	int j;
	double sum_pow2distCM = 0.0; // acumulador cuadrados de distancias al centro de masas (en km)
	double distanceact = 0.0;
	double distancemax = 0.0;
	
	// Initializations
	mobVars.dif_btss = mobVars.dif_muns = mobVars.dif_states = 0;
	mobVars.masscenter_utmx = mobVars.masscenter_utmy = 0;
	mobVars.radius = mobVars.diam_areainf = 0;

	telMonth.parse(inputs[0].kvs[0].key);
	
	// Number of positions with bts
	mobVars.num_pos = inputs[0].num_kvs;
	
	// Temporal vector for compile btss, lacs and states
	tempMob.btssSetLength(mobVars.num_pos);
	tempMob.munsSetLength(mobVars.num_pos);
	tempMob.statesSetLength(mobVars.num_pos);
	tempMob.coord_utmxSetLength(mobVars.num_pos);
	tempMob.coord_utmySetLength(mobVars.num_pos);
	// Vectors are initialized to 0
	for ( j=0; j<mobVars.num_pos; j++) 
	{
		tempMob.btss[j] = 0;
		tempMob.muns[j] = 0;
		tempMob.states[j] = 0;
		tempMob.coord_utmx[j] = 0;
		tempMob.coord_utmy[j] = 0;
	}
	
	// Calls in a period (Month and working/weekend day)
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		cellInfo.parse(inputs[0].kvs[i].value);
			
		// BTS
		for ( j=0; j<mobVars.dif_btss; j++) 
		{
			if (tempMob.btss[j] == cellInfo.bts)
				break;
		}
		if (j==mobVars.dif_btss)  // Bts is not in the vector
		{ 
			mobVars.dif_btss ++;
			tempMob.btss[j] = cellInfo.bts;
			tempMob.coord_utmx[j] = cellInfo.posx;
			tempMob.coord_utmy[j] = cellInfo.posy;
		}
		// LAC
		for ( j=0; j<mobVars.dif_muns; j++) 
		{
			if (tempMob.muns[j] == cellInfo.mun)
				break;
		}
		if (j==mobVars.dif_muns)  // Lac is not in the vector
		{
			mobVars.dif_muns ++;
			tempMob.muns[j] = cellInfo.mun;
		}
		// STATE
		for (j=0; j<mobVars.dif_states; j++) 
		{
			if (tempMob.states[j] == cellInfo.sta)
				break;
		}
		if (j==mobVars.dif_states) // State is not in the vector
		{ 
			mobVars.dif_states ++;
			tempMob.states[j] = cellInfo.sta;
		}
		// Mass center accumulation
		mobVars.masscenter_utmx += cellInfo.posx;
		mobVars.masscenter_utmy += cellInfo.posy;	
	}
	
	// Mass center final calculation
	mobVars.masscenter_utmx /= mobVars.num_pos;
	mobVars.masscenter_utmy /= mobVars.num_pos;	
	
	// Spin radius: weigthed mean of all distances to the mass center 
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		cellInfo.parse(inputs[0].kvs[i].value);	
		sum_pow2distCM += pow((cellInfo.posx-mobVars.masscenter_utmx),2)+pow((cellInfo.posy-mobVars.masscenter_utmy),2);
	}
	mobVars.radius = sqrt(sum_pow2distCM/mobVars.num_pos );
	
	// Influence area diameter (max distances between all BTSs) 
	// Only different bts are considered
	for (int a=0; a<mobVars.dif_btss; a++) 
	{
		for (int b=a+1; b<mobVars.dif_btss; b++) 
		{
			distanceact = sqrt(pow((tempMob.coord_utmx[a]-tempMob.coord_utmx[b]),2)+pow((tempMob.coord_utmy[a]-tempMob.coord_utmy[b]),2));
			if (distanceact > distancemax)
				distancemax = distanceact;
		}
	}
	mobVars.diam_areainf = distancemax;
	
	phone.value = telMonth.phone;
	mobVars.month = telMonth.month;
	mobVars.workingday = telMonth.workingday;
	writer[0]->emit(&phone,&mobVars);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------
void MACRO_mobmx_MobVar_fusiontotalvars::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs
	MRData_UInt phone;
	MRData_MobVars period_vars;
	// Outputs
	MRData_Mob_VI_MobVars totalVars;

	// Intermediate
	MRData_Mob_VI_MobVars aux;
	int min_month, min_elem;
	int item_month = 0;

	phone.parse(inputs[0].kvs[0].key);
	aux.varsSetLength(0);
	totalVars.varsSetLength(0);
	
	// Initialization of vectors
	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		period_vars.parse(inputs[0].kvs[i].value);
        aux.varsAdd()->copyFrom(&period_vars);
	}
	// Sort vectors
	for(int nElemAdd=0; nElemAdd<aux.vars_length; nElemAdd++)
	{
		min_month = 26;
		min_elem = -1;
		for(int elem=0; elem < aux.vars_length; elem++)
		{
			if(aux.vars[elem].month == 0){ continue;}
			item_month = (aux.vars[elem].month * 2) + abs((int)aux.vars[elem].workingday - 1);
			if(item_month < min_month)
			{
				min_month = item_month;
				min_elem = elem;
			}
		}
		totalVars.varsAdd()->copyFrom(&aux.vars[min_elem]);
		aux.vars[min_elem].month = 0;
	}
	writer[0]->emit(&phone,&totalVars);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------
int MACRO_mobmx_MobVar_indVars_out::parseOut( KVSet kvSet , FILE *file)
{
	// Inputs
	MRData_UInt phoneId_key;
	MRData_Mob_VI_MobVars totalVars;
	
	int counter=0;
	
	for (int kv = 0 ; kv < kvSet.num_kvs ; kv++)
	{
		phoneId_key.parse(kvSet.kvs[kv].key);
		totalVars.parse( kvSet.kvs[kv].value );
		
		// TODO: Modificar a parámetros de configuración o algo así
		for(int nMon=1; nMon<=6; nMon++)
		{
			bool exists = false;
			counter += fprintf(file, "%lu|%d" , phoneId_key.value, nMon);
			for(int j=0; j<totalVars.vars_length; j++)
			{
				if(totalVars.vars[j].month == nMon && totalVars.vars[j].workingday == 1)
				{
					counter += fprintf(file, "|%d", totalVars.vars[j].num_pos);
					counter += fprintf(file, "|%d", totalVars.vars[j].dif_btss);
					counter += fprintf(file, "|%d", totalVars.vars[j].dif_muns);
					counter += fprintf(file, "|%d", totalVars.vars[j].dif_states);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmx);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmy);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].radius);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].diam_areainf);
					exists = true;
					break;
				}
			}
			if(!exists)
			{
				counter += fprintf(file,"|-1|-1|-1|-1|-1|-1|-1|-1");
			}
			exists = false;
			for(int j=0; j<totalVars.vars_length; j++)
			{
				if(totalVars.vars[j].month == nMon && totalVars.vars[j].workingday == 0)
				{
					counter += fprintf(file, "|%d", totalVars.vars[j].num_pos);
					counter += fprintf(file, "|%d", totalVars.vars[j].dif_btss);
					counter += fprintf(file, "|%d", totalVars.vars[j].dif_muns);
					counter += fprintf(file, "|%d", totalVars.vars[j].dif_states);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmx);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmy);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].radius);
					counter += fprintf(file, "|%.4f", totalVars.vars[j].diam_areainf);
					exists = true;
					break;
				}
			}
			if(!exists)
			{
				counter += fprintf(file,"|-1|-1|-1|-1|-1|-1|-1|-1");
			}
			counter += fprintf(file,"\n");
		}
	}
	return counter;
}
// -----------------------
int MACRO_mobmx_MobVar_indVars_out::parseOutFinish( FILE *file)
{
	return 0;
}

////// ****** Processing of all months as a period ****** //////
void MACRO_mobmx_MobVar_delete_period::run( KVSet* inputs, int num_inputs , KVWriterInterface **writer , int num_outputs)
{
	// Inputs - Outputs
	MRData_TelMonth telmonth;
	MRData_CellMx cell;

	for(int i=0; i<inputs[0].num_kvs; i++)
	{
		telmonth.parse(inputs[0].kvs[i].key);
		cell.parse(inputs[0].kvs[i].value);
		telmonth.month = 1;
		writer[0]->emit(&telmonth,&cell);
	}
}
int MACRO_mobmx_MobVar_indVars_accumulated_out::parseOut( KVSet kvSet , FILE *file)
{
	// Inputs
	MRData_UInt phoneId_key;
	MRData_Mob_VI_MobVars totalVars;
	
	int counter=0;
	
	for (int kv = 0 ; kv < kvSet.num_kvs ; kv++)
	{
		phoneId_key.parse(kvSet.kvs[kv].key);
		totalVars.parse( kvSet.kvs[kv].value );
		
		counter += fprintf(file, "%lu" , phoneId_key.value);

		bool exists = false;
		for(int j=0; j<totalVars.vars_length; j++)
		{
			if(totalVars.vars[j].workingday == 1)
			{
				counter += fprintf(file, "|%d", totalVars.vars[j].num_pos);
				counter += fprintf(file, "|%d", totalVars.vars[j].dif_btss);
				counter += fprintf(file, "|%d", totalVars.vars[j].dif_muns);
				counter += fprintf(file, "|%d", totalVars.vars[j].dif_states);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmx);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmy);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].radius);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].diam_areainf);
				exists = true;
				break;
			}
		}
		if(!exists)
		{
			counter += fprintf(file,"|-1|-1|-1|-1|-1|-1|-1|-1");
		}
		exists = false;
		for(int j=0; j<totalVars.vars_length; j++)
		{
			if(totalVars.vars[j].workingday == 0)
			{
				counter += fprintf(file, "|%d", totalVars.vars[j].num_pos);
				counter += fprintf(file, "|%d", totalVars.vars[j].dif_btss);
				counter += fprintf(file, "|%d", totalVars.vars[j].dif_muns);
				counter += fprintf(file, "|%d", totalVars.vars[j].dif_states);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmx);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].masscenter_utmy);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].radius);
				counter += fprintf(file, "|%.4f", totalVars.vars[j].diam_areainf);
				exists = true;
				break;
			}
		}
		if(!exists)
		{
			counter += fprintf(file,"|-1|-1|-1|-1|-1|-1|-1|-1");
		}
		counter += fprintf(file,"\n");
	}
	return counter;
}
// -----------------------
int MACRO_mobmx_MobVar_indVars_accumulated_out::parseOutFinish( FILE *file)
{
	return 0;
}