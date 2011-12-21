

----------------------------------------
SIMPLE MOBILITY
----------------------------------------

SAMSON demo consisting in receving a continuous stream of 300K events per second
with an update of the user's (40M set of users ) position.

First, we send a set of "commands" to define home & work for all users
Second, we send all cdrs in continuous
We can check messages and levels at the output

-----------------------------------------

** demo_init

   Init cluster with necessary operations for this demo

** demo_send_fake_data_commands
** demo_send_fake_data_cdrs

   Send commands to define users home  & work
   Send cdrs to update users position	

** demo_visualitzation_txt_levels*
** demo_visualitzation_txt_messages*
** demo_visualitzation_users_at_home*
** demo_visualitzation_users_at_work*

   Visualitzation of different information
   -> txt_levels: 
   	  			  #users with home,work defined ( command received )
				  #users currently at their home,work
   -> txt_messages: Messages with notifications of area created, user in out area
   -> users_at_home, users_at_work ... graphical visualialitzation 		 