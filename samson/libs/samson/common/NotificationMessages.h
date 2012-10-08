
#ifndef _H_NotificationMessages
#define _H_NotificationMessages

// Definition of the Notification messages ( const char * )


// Notification to report status-update to all connected delilahs
#define notification_update_status                    "notification_update_status"

#define notification_samson_worker_check_finish_tasks "notification_samson_worker_check_finish_tasks"


#define notification_samson_worker_send_message         "notification_samson_worker_send_message"


#define notification_review_worker_command_manager    "notification_review_worker_command_manager"

// Notification to review stream manager
#define notification_review_stream_manager_fast       "notification_review_stream_manager_fast"
#define notification_review_stream_manager_save_state "notification_review_stream_manager_save_state"

// Notification that a network element has been disconnected
#define notification_network_diconnected              "notification_network_diconnected"

// Notification to recover state from files
#define notification_recoverStateFromDisk             "notification_recoverStateFromDisk"


// Notification to review repeat commands in delilha
#define notification_delilah_review_repeat_tasks      "notification_delilah_review_repeat_tasks"

// Notification to review push manager in the worker
#define notification_worker_block_manager_review      "notification_worker_block_manager_review"

#define notification_send_packet                      "notification_send_packet" // Notification to send a packet
#define notification_packet_received                  "notification_packet_received" // Notification to receive a packet

#endif  // ifndef _H_NotificationMessages
