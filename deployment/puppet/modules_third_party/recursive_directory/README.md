puppet-recursive_directory
==========================

Puppet module to allow for files to be created recursively from a folder of templates

The benefit here is that you no longer need to define file resources for each and every template file

This should help to substantially shorten manifests that include lots of template files

usage
=====
```
recursive_directory {'some_unique_title':
      source_dir => 'custom_module/source_dir',
      dest_dir  => '/tmp',
      file_mode  => '0644',
      owner      => 'root',
      group      => 'root'
}
```
> This will copy all files from <module_path>custom_module/templates/source_dir folder
> and interpolate variables the same as when using the template() function inside of the
> manifest itself and put them into /tmp

parameter documentation
=======================
**source_dir**  

>The module_name followed by a subfolder inside of <module_name>/templates
>If source_dir is simply the modulename, recursive_directory will interpolate and
>create file resources for all files in <module_name>  
>**required: true**
    
**dest_dir**  

>The fully qualified path on the client system where the interpolated templates and files
>should be created  
>**required: true**
    
**file_mode**  

>The file mode for all of the files  
>**required: false**  
>**defaut: 0600**
    
**dir_mode**  

>The file mode for all of the directories
>**required: false**  
>**defaut: 0700**  

**owner**
   
>The owner of the file  
>**required: false**  
>**default: 'nobody'**
    
**group**
   
>The owner of the file  
>**required: false**  
>**default: 'nobody'**

testing
=======

rake spec in the root checkout of the module
