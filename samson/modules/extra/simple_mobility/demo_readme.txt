
General setup
----------------------------------

** Setup demo at delilah with

init_stream simple_mobility.demo

** Send all commands to define home and work areas for all users with 

simple_mobility_generator -commands | samsonPush demo.01.in_txt_commands

** Send CDRs in continuous mode

simple_mobility_generator | samsonPush  demo.02.in_txt_cdrs


Visualization
-------------------------------------------

** General messages

samsonPop demo.30.messages

** Levels

samsonPop demo.99.plot.04.txt_notifications | samsonLevelMonitor -concept work_defined -title "Work defined" &
samsonPop demo.99.plot.04.txt_notifications | samsonLevelMonitor -concept home_defined -title "Home defined" &
samsonPop demo.99.plot.04.txt_notifications | samsonLevelMonitor -concept work -title "Users at work" &
samsonPop demo.99.plot.04.txt_notifications | samsonLevelMonitor -concept home -title "Users at home" &



Desktop test of demo ( Less volume test )
-------------------------------------------

simple_mobility_generator -users 100000 -commands | samsonPush demo.01.in_txt_commands -v
simple_mobility_generator -users 100000 -rate 1000 | samsonPush demo.02.in_txt_cdrs -v -buffer_size 10000 -timeout 1