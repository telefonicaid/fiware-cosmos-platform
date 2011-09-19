
#ifndef _H_NotificationMessages
#define _H_NotificationMessages

// Definition of the Notification messages ( const char * )


#define notification_samson_worker_send_status_update   "notification_samson_worker_send_status_update"
#define notification_samson_worker_send_trace           "notification_samson_worker_send_trace"
#define notification_send_to_worker                     "notification_send_to_worker"

#define notification_samson_worker_check_finish_tasks   "notification_samson_worker_check_finish_tasks"

#define notification_samson_worker_send_packet          "notification_samson_worker_send_packet"


// Notification to review worker_task ( in Stream manager )
#define notification_review_stream_manager                "notification_review_stream_manager"

// Notification to review timeout in SamsonPushBuffer

#define notification_review_timeOut_SamsonPushBuffer        "notification_review_timeOut_SamsonPushBuffer"

// Notification to review old block files
#define notification_block_manager_review                   "notification_block_manager_review"

#endif
