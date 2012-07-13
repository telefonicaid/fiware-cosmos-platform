Init scripts for putting directly into samson packaging
=======================================================

   - Samson needs to configure shared memory before starting 'samsonSpawner'

   - That means that we'll split samson init process into two different scripts:

                     
                samson_presets : This one will only launch at start
                                 and will set shared memory accordingly.
                
                samson : This will have the start/stop sequence

   - When packaging, must be assured that 'samson_presets' runs BEFORE 'samson' 

