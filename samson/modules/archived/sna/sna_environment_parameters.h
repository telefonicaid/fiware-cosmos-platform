/*
 * sna_environment_parameters.h
 *
 *  Created on: 23/02/2011
 *      Author: jges
 */

#ifndef SNA_ENVIRONMENT_PARAMETERS_H_
#define SNA_ENVIRONMENT_PARAMETERS_H_

#define SNA_PARAMETER_CDR_FORMAT									"sna.cdr_format"
#define SNA_PARAMETER_CDR_FORMAT_DEFAULT							"TME"
#define SNA_PARAMETER_CDR_FORMAT_TME								"TME"
#define SNA_PARAMETER_CDR_FORMAT_JAJAH								"JAJAH"
#define SNA_PARAMETER_CDR_FORMAT_MX									"MX"
#define SNA_PARAMETER_CDR_FORMAT_TASA								"TASA"
#define SNA_PARAMETER_CDR_FORMAT_TEMM_VOZ							"TEMM_VOZ"
#define SNA_PARAMETER_CDR_FORMAT_TEMM_SMS							"TEMM_SMS"
#define SNA_PARAMETER_CDR_FORMAT_TEMM_MMS							"TEMM_MMS"

#define SNA_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION					"sna.num_cdrs_for_strong_connection"
#define SNA_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION_DEFAULT			4
#define SNA_PARAMETER_HISTORY_MONTH_IMPORTANCE						"sna.history_graph_importance"
#define SNA_PARAMETER_HISTORY_MONTH_IMPORTANCE_DEFAULT				0.7
#define SNA_PARAMETER_WEIGHT_LINK_TO_DIE								"sna.weight_link_to_die"
#define SNA_PARAMETER_WEIGHT_LINK_TO_DIE_DEFAULT						85
#define SNA_PARAMETER_SPIKES_PUNCTUAL_RELATIONSHIP					"sna.number_spikes_to_punctual_relationship"
#define SNA_PARAMETER_SPIKES_PUNCTUAL_RELATIONSHIP_DEFAULT			1
#define SNA_PARAMETER_THRESHOLD_TO_FUSE_CLIQUES						"sna.threshold_to_fuse_cliques"
#define SNA_PARAMETER_THRESHOLD_TO_FUSE_CLIQUES_DEFAULT				0.7
#define SNA_PARAMETER_THRESHOLD_TO_ADD_ASSOCIATED_NODES				"sna.threshold_to_add_associated_nodes"
#define SNA_PARAMETER_THRESHOLD_TO_ADD_ASSOCIATED_NODES_DEFAULT		0.6
#define SNA_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK				"sna.cliques_penalization_missing_link"
#define SNA_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK_DEFAULT		1.0
#define SNA_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK_EXTERNAL		"sna.cliques_penalization_missing_link_external"
#define SNA_PARAMETER_CLIQUES_PENALIZATION_MISSION_LINK_EXTERNAL_DEFAULT	0.0
#define SNA_PARAMETER_MAX_STRONG_CONNECTIONS							"sna.max_strong_connections_per_node"
#define SNA_PARAMETER_MAX_STRONG_CONNECTIONS_DEFAULT					50
#define SNA_PARAMETER_MAX_CONNECTIONS									"sna.max_connections_per_node"
#define SNA_PARAMETER_MAX_CONNECTIONS_DEFAULT							400
#define SNA_PARAMETER_COMMUNITY_FUSITON_MIN_OVERLAPING				"sna.community_fusion_min_overlaping"
#define SNA_PARAMETER_COMMUNITY_FUSITON_MIN_OVERLAPING_DEFAULT		0.8
#define SNA_PARAMETER_COMMUNITY_FUSITON_NEW_THRESHOLD					"sna.community_fusion_new_threshold"
#define SNA_PARAMETER_COMMUNITY_FUSITON_NEW_THRESHOLD_DEFAULT			0.5
#define SNA_PARAMETER_COMMUNITY_MAX_COMMUNITY_LINK					"sna.community_max_community_links"
#define SNA_PARAMETER_COMMUNITY_MAX_COMMUNITY_LINK_DEFAULT			10
#define SNA_PARAMETER_COMMUNITIES_SEP	"sna.communities_sep"
#define SNA_PARAMETER_COMMUNITIES_SEP_DEFAULT	"|"

#define SNA_PARAMETER_NUM_EXAMPLE_FAKE_GENERATOR			"sna.num_example_fake_generator"
#define SNA_PARAMETER_NUM_EXAMPLE_FAKE_GENERATOR_DEFAULT	1
#define SNA_PARAMETER_MIN_WEIGHTED_LINK_WEIGH "sna.min_weighted_link_weight"
#define SNA_PARAMETER_MIN_WEIGHTED_LINK_WEIGH_DEFAULT 150

#define SNA_PARAMETER_GENERATE_SINGLE_CLIQUE_NODES	"sna.generate_single_clique_nodes"
#define SNA_PARAMETER_GENERATE_SINGLE_CLIQUE_NODES_DEFAULT	"1;2"

#define SNA_PARAMETER_NODEIDSTR				"sna.nodeId"
#define SNA_PARAMETER_NODEIDSTR_DEFAULT		"null"

#define SNA_PARAMETER_NODES				"sna.nodeId"
#define SNA_PARAMETER_NODES_DEFAULT				"null"

#define SNA_FLAG_TRUE								"true"
#define SNA_FLAG_FALSE								"false"

#define SNA_FLAG_ONLY_EXTREME				"sna.only_extreme"
#define SNA_FLAG_FUSING_COMMUNITIES		"sna.fusing_communities"
#define SNA_FLAG_NO_DUPLICATE												"sna.no_duplicate"
#define SNA_FLAG_NO_DUPLICATE_DEFAULT										0
#define SNA_FLAG_NODES											"sna.customers"
#define SNA_FLAG_PARSE_DIR													"sna.parse_dir"
#define SNA_FLAG_PARSE_DIR_DEFAULT											0
#define SNA_FLAG_TELEFONICA_NODES											"sna.telefonica_nodes"
#define SNA_FLAG_TELEFONICA_NODES_DEFAULT											"true"
#define SNA_FLAG_TELEFONICA_NODES_TRUE											"true"
#define SNA_FLAG_TELEFONICA_NODES_FALSE											"true"
#define SNA_FLAG_DUPLICATES														"sna.duplicates"
#define SNA_FLAG_DUPLICATES_DEFAULT												"false"
#define SNA_FLAG_EXTERN_MARK													"sna.extern_mark"
#define SNA_FLAG_EXTERN_MARK_DEFAULT											"false"

#endif /* SNA_ENVIRONMENT_PARAMETERS_H_ */
