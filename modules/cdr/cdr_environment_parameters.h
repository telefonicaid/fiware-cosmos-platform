/*
 * sna_environment_parameters.h
 *
 *  Created on: 23/02/2011
 *      Author: jges
 */

#ifndef GST_ENVIRONMENT_PARAMETERS_H_
#define GST_ENVIRONMENT_PARAMETERS_H_


// Possible mobility scopes
typedef enum {
	LOC_NO_INFORMADO = 0,
	LOC_NO_APLICA,
	LOC_GENERICO,
	LOC_OTRO,
	LOC_NO_ROAMING,
	LOC_ROAMING_VISITANTE,
	LOC_ROAMING_INTRARREGIONAL,
	LOC_ROAMING_INTERREGIONAL,
	LOC_ROAMING_NACIONAL,
	LOC_ROAMING_INTERNACIONAL,
	LOC_ROAMING_MUNDIAL,
	LOC_MOB_SCOPES_NUMBER
} enumScopes_MX;


//
// MEXICO
//



// Possible mobility degrees
typedef enum
{
   MD_UNKNOWN = 0,
   MD_REDUCED,
   MD_PROVINCIAL,
   MD_NATIONAL,
   MD_INTERNATIONAL,
   MD_MIXED,
   MD_NUMBER_OF_DEGREES
} enumMobDegrees_MX;


#define CONF_MOB_CELL_LENGTH        8
#define CONF_MOB_PHONE_LENGTH       10



#define GST_PARAMETER_MIN_PHONE_DIGITS					"gst.min_phone_digits"
#define GST_PARAMETER_MIN_PHONE_DIGITS_DEFAULT			10
#define GST_PARAMETER_MAX_PHONE_DIGITS					"gst.max_phone_digits"
#define GST_PARAMETER_MAX_PHONE_DIGITS_DEFAULT			10

#define GST_FLAG_OLD_DATE												"gst.old_date"
#define GST_FLAG_OLD_DATE_DEFAULT										0

#define GST_PARAMETER_CDR_FORMAT									"gst.cdr_format"
#define GST_PARAMETER_CDR_FORMAT_DEFAULT							"TME"
#define GST_PARAMETER_CDR_FORMAT_TME								"TME"
#define GST_PARAMETER_CDR_FORMAT_JAJAH								"JAJAH"
#define GST_PARAMETER_CDR_FORMAT_MX									"MX"
#define GST_PARAMETER_CDR_FORMAT_TASA								"TASA"
#define GST_PARAMETER_CDR_FORMAT_TEMM_VOZ							"TEMM_VOZ"
#define GST_PARAMETER_CDR_FORMAT_TEMM_SMS							"TEMM_SMS"
#define GST_PARAMETER_CDR_FORMAT_TEMM_MMS							"TEMM_MMS"




#if 0



#define GST_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION					"gst.num_cdrs_for_strong_connection"
#define GST_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION_DEFAULT			4
#define GST_PARAMETER_HISTORY_MONTH_IMPORTANCE						"gst.history_graph_importance"
#define GST_PARAMETER_HISTORY_MONTH_IMPORTANCE_DEFAULT				0.7
#define GST_PARAMETER_WEIGHT_LINK_TO_DIE								"gst.weight_link_to_die"
#define GST_PARAMETER_WEIGHT_LINK_TO_DIE_DEFAULT						80
#define GST_PARAMETER_SPIKES_PUNCTUAL_RELATIONSHIP					"gst.number_spikes_to_punctual_relationship"
#define GST_PARAMETER_SPIKES_PUNCTUAL_RELATIONSHIP_DEFAULT			1
#define GST_PARAMETER_THRESHOLD_TO_FUSE_CLIQUES						"gst.threshold_to_fuse_cliques"
#define GST_PARAMETER_THRESHOLD_TO_FUSE_CLIQUES_DEFAULT				0.7
#define GST_PARAMETER_THRESHOLD_TO_ADD_ASSOCIATED_NODES				"gst.threshold_to_add_associated_nodes"
#define GST_PARAMETER_THRESHOLD_TO_ADD_ASSOCIATED_NODES_DEFAULT		0.5
#define GST_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK				"gst.cliques_penalization_missing_link"
#define GST_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK_DEFAULT		1.0
#define GST_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK_EXTERNAL		"gst.cliques_penalization_missing_link_external"
#define GST_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK_EXTERNAL_DEFAULT	0.0
#define GST_PARAMETER_MAX_STRONG_CONNECTIONS							"gst.max_strong_connections_per_node"
#define GST_PARAMETER_MAX_STRONG_CONNECTIONS_DEFAULT					50
#define GST_PARAMETER_MAX_CONNECTIONS									"gst.max_connections_per_node"
#define GST_PARAMETER_MAX_CONNECTIONS_DEFAULT							400
#define GST_PARAMETER_COMMUNITY_FUSITON_MIN_OVERLAPING				"gst.community_fusion_min_overlaping"
#define GST_PARAMETER_COMMUNITY_FUSITON_MIN_OVERLAPING_DEFAULT		0.8
#define GST_PARAMETER_COMMUNITY_FUSITON_NEW_THRESHOLD					"gst.community_fusion_new_threshold"
#define GST_PARAMETER_COMMUNITY_FUSITON_NEW_THRESHOLD_DEFAULT			0.5
#define GST_PARAMETER_COMMUNITY_MAX_COMMUNITY_LINK					"gst.community_max_community_links"
#define GST_PARAMETER_COMMUNITY_MAX_COMMUNITY_LINK_DEFAULT			10
#define GST_PARAMETER_COMMUNITIES_SEP	"gst.communities_sep"
#define GST_PARAMETER_COMMUNITIES_SEP_DEFAULT	"|"

#define GST_PARAMETER_NUM_EXAMPLE_FAKE_GENERATOR			"gst.num_example_fake_generator"
#define GST_PARAMETER_NUM_EXAMPLE_FAKE_GENERATOR_DEFAULT	1
#define GST_PARAMETER_MIN_WEIGHTED_LINK_WEIGH "gst.min_weighted_link_weight"
#define GST_PARAMETER_MIN_WEIGHTED_LINK_WEIGH_DEFAULT 150

#define GST_PARAMETER_GENERATE_SINGLE_CLIQUE_NODES	"gst.generate_single_clique_nodes"
#define GST_PARAMETER_GENERATE_SINGLE_CLIQUE_NODES_DEFAULT	"1;2"

#define GST_PARAMETER_NODEIDSTR				"gst.nodeId"
#define GST_PARAMETER_NODEIDSTR_DEFAULT		"null"

#define GST_PARAMETER_NODES				"gst.nodeId"
#define GST_PARAMETER_NODES_DEFAULT				"null"

#define GST_FLAG_TRUE								"true"
#define GST_FLAG_FALSE								"false"

#define GST_FLAG_ONLY_EXTREME				"gst.only_extreme"
#define GST_FLAG_FUSING_COMMUNITIES		"gst.fusing_communities"
#define GST_FLAG_NO_DUPLICATE												"gst.no_duplicate"
#define GST_FLAG_NO_DUPLICATE_DEFAULT										0
#define GST_FLAG_NODES											"gst.customers"
#define GST_FLAG_PARSE_DIR													"gst.parse_dir"
#define GST_FLAG_PARSE_DIR_DEFAULT											0
#define GST_FLAG_TELEFONICA_NODES											"gst.telefonica_nodes"
#define GST_FLAG_TELEFONICA_NODES_DEFAULT											"true"
#define GST_FLAG_TELEFONICA_NODES_TRUE											"true"
#define GST_FLAG_TELEFONICA_NODES_FALSE											"true"
#define GST_FLAG_DUPLICATES														"gst.duplicates"
#define GST_FLAG_DUPLICATES_DEFAULT												"false"
#define GST_FLAG_EXTERN_MARK													"gst.extern_mark"
#define GST_FLAG_EXTERN_MARK_DEFAULT											"false"

#endif


#endif /* GST_ENVIRONMENT_PARAMETERS_H_ */
