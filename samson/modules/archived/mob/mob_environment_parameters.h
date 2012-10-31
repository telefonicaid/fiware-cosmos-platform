/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/*
 * mob_environment_parameters.h
 *
 *  Created on: 23/02/2011
 *      Author: jges
 */

#ifndef mob_ENVIRONMENT_PARAMETERS_H_
#define mob_ENVIRONMENT_PARAMETERS_H_

#define MOB_PARAMETER_CONF_TIMESLOT_HOME									"mob.conf_timeslot_home"
#define MOB_PARAMETER_CONF_TIMESLOT_HOME_DEFAULT							"2|0001111 21:00:00 23:59:59|1100000 08:00:00 12:59:59"

#define MOB_PARAMETER_CONF_TIMESLOT_WORK									"mob.conf_timeslot_work"
#define MOB_PARAMETER_CONF_TIMESLOT_WORK_DEFAULT							"1|0011111 09:00:00 14:59:59"

#define MOB_PARAMETER_MINMONTHCALLS_IN "mob.parameter_minMonthCalls_in"
#define MOB_PARAMETER_MINMONTHCALLS_IN_DEFAULT 2

#define MOB_PARAMETER_MINMONTHCALLS_NAT_HOME "mob.parameter_minMonthCalls_nat_home"
#define MOB_PARAMETER_MINMONTHCALLS_NAT_HOME_DEFAULT 5

#define MOB_PARAMETER_MINMONTHCALLS_STA "mob.parameter_minMonthCalls_sta"
#define MOB_PARAMETER_MINMONTHCALLS_STA_DEFAULT 11

#define MOB_PARAMETER_MINMONTHCALLS_LAC "mob.parameter_minMonthCalls_lac"
#define MOB_PARAMETER_MINMONTHCALLS_LAC_DEFAULT 7

#define MOB_PARAMETER_MINPERLACSTACALLS "mob.parameter_minPerLacStaCalls"
#define MOB_PARAMETER_MINPERLACSTACALLS_DEFAULT 80

#define MOB_PARAMETER_NUMDAYSMIN_CELL_HOME "mob.parameter_numDaysMin_cell_home"
#define MOB_PARAMETER_NUMDAYSMIN_CELL_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_CELL_HOME "mob.parameter_freqMin_cell_home"
#define MOB_PARAMETER_FREQMIN_CELL_HOME_DEFAULT 0.0

#define MOB_PARAMETER_NUMDAYSMIN_BTS_HOME "mob.parameter_numDaysMin_bts_home"
#define MOB_PARAMETER_NUMDAYSMIN_BTS_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_BTS_HOME "mob.parameter_freqMin_bts_home"
#define MOB_PARAMETER_FREQMIN_BTS_HOME_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_LAC_HOME "mob.numDaysMin_lac_home"
#define MOB_PARAMETER_NUMDAYSMIN_LAC_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_LAC_HOME "mob.freqMin_lac_home"
#define MOB_PARAMETER_FREQMIN_LAC_HOME_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_STA_HOME "mob.numDaysMin_sta_home"
#define MOB_PARAMETER_NUMDAYSMIN_STA_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_STA_HOME "mob.freqMin_sta_home"
#define MOB_PARAMETER_FREQMIN_STA_HOME_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_CELL_WORK "mob.numDaysMin_cell_work"
#define MOB_PARAMETER_NUMDAYSMIN_CELL_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_CELL_WORK "mob.freqMin_cell_work"
#define MOB_PARAMETER_FREQMIN_CELL_WORK_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_BTS_WORK "mob.numDaysMin_bts_work"
#define MOB_PARAMETER_NUMDAYSMIN_BTS_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_BTS_WORK "mob.freqMin_bts_work"
#define MOB_PARAMETER_FREQMIN_BTS_WORK_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_LAC_WORK "mob.numDaysMin_lac_work"
#define MOB_PARAMETER_NUMDAYSMIN_LAC_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_LAC_WORK "mob.freqMin_lac_work"
#define MOB_PARAMETER_FREQMIN_LAC_WORK_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_STA_WORK "mob.numDaysMin_sta_work"
#define MOB_PARAMETER_NUMDAYSMIN_STA_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_STA_WORK "mob.freqMin_sta_work"
#define MOB_PARAMETER_FREQMIN_STA_WORK_DEFAULT 0








#define MOB_FLAG_TRUE								"true"
#define MOB_FLAG_FALSE								"false"

#define MOB_FLAG_ONLY_EXTREME				"mob.only_extreme"
#define MOB_FLAG_FUSING_COMMUNITIES		"mob.fusing_communities"
#define MOB_FLAG_NO_DUPLICATE												"mob.no_duplicate"
#define MOB_FLAG_NO_DUPLICATE_DEFAULT										0
#define MOB_FLAG_NODES											"mob.customers"
#define MOB_FLAG_PARSE_DIR													"mob.parse_dir"
#define MOB_FLAG_PARSE_DIR_DEFAULT											0
#define MOB_FLAG_TELEFONICA_NODES											"mob.telefonica_nodes"
#define MOB_FLAG_TELEFONICA_NODES_DEFAULT											"true"
#define MOB_FLAG_TELEFONICA_NODES_TRUE											"true"
#define MOB_FLAG_TELEFONICA_NODES_FALSE											"true"
#define MOB_FLAG_DUPLICATES														"mob.duplicates"
#define MOB_FLAG_DUPLICATES_DEFAULT												"false"
#define MOB_FLAG_EXTERN_MARK													"mob.extern_mark"
#define MOB_FLAG_EXTERN_MARK_DEFAULT											"false"

#endif /* mob_ENVIRONMENT_PARAMETERS_H_ */
