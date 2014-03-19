require 'puppet'
require 'find'

module Puppet::Parser::Functions
    # expects an args containing:
    # args[0] 
    # - The source module and directory inside of templates
    # - We will insert templates/ after the module name in this code
    # - required: true
    #
    # args[1]
    # - The destination directory for the interpolated templates to
    # - go on the client machine
    # - required: true
    #
    # args[2]
    # - The file mode for the finished files on the client
    # - required: false
    # - default: 0600
    #
    # args[3]
    # - The owner of the file
    # - required: false
    # - default: owner of puppet running process
    #
    # args[4]
    # - The group ownership of the file
    # - required: false
    # - default: owner of puppet running process
    #
    newfunction(:recurse_directory, :type => :rvalue) do |args|
    source_dir = args[0]
    destination_dir = args[1]
    file_mode = args[2]
    if not file_mode or file_mode == ''
        file_mode = '0600'
    end
    file_owner = args[3]
    file_group = args[4]
    dir_mode = args[5]
    creatable_resources = Hash.new
    source_dir_array = source_dir.split(/\//)
    #
    # insert /templates to the modulename as our base search path
    #
    source_dir_array[0] = "#{source_dir_array[0]}/templates"
    search_path = source_dir_array.join('/')

    moduledirs = Puppet[:modulepath].split(/:/)
    # **** Cosmos fix to allow for multiple module paths. ***
    file_paths = moduledirs.map{|mdir| "#{mdir}/#{search_path}"}
    debug "Searching in #{file_paths.join(',')}"
    file_paths.each do |file_path|
      Find.find(file_path) do |f|
          full_path = f
          f.slice!(file_path + "/")
          if f == file_path or f == '' or !f
              next
          end
          if not File.directory?("#{file_path}/#{f}")
              ensure_mode = 'file'
              title = f.gsub(/\.erb$/,'')
              debug("File in loop #{f}")
              debug("Title in loop #{title}")
              destination_full_path = "#{destination_dir}/#{title}"
              #file = "#{template_path}/#{f}"
              #Cosmos fix
              file = "#{file_path}/#{f}"
              debug "Retrieving template #{file}"

              wrapper = Puppet::Parser::TemplateWrapper.new(self)
              wrapper.file = file
              begin
              wrapper.result
              rescue => detail
              info = detail.backtrace.first.split(':')
              raise Puppet::ParseError,
                  "Failed to parse template #{file}:\n  Filepath: #{info[0]}\n  Line: #{info[1]}\n  Detail: #{detail}\n"
              end
              template_content = wrapper.result

              creatable_resources[destination_full_path] = {
                  'ensure' => ensure_mode,
                  'content' => template_content,
              }
              if file_owner
                  creatable_resources[destination_full_path]['owner'] = file_owner
              end
              if file_group
                  creatable_resources[destination_full_path]['group'] = file_group
              end
              if file_mode
                  creatable_resources[destination_full_path]['mode'] = file_mode
              end
          elsif File.directory?("#{file_path}/#{f}") and f != '.' and f != '..'
              title = f
              destination_full_path = "#{destination_dir}/#{title}"
              creatable_resources[destination_full_path] = {
                  'ensure' => 'directory',
                  'owner' => file_owner,
                  'group' => file_group,
              }
              if dir_mode
                  creatable_resources[destination_full_path]['mode'] = dir_mode
              end
          end

      end
      debug("Source Dir #{source_dir}")
      debug("Destination Dir #{destination_dir}")
      debug("Module Dirs #{moduledirs.join(',')}")
      debug("File Path #{file_path}")
      debug("Creatable Resources #{creatable_resources}")
    end
    return creatable_resources
    end

end
