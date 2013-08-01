#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

require 'net/http'
require 'uri'

module Puppet::Parser::Functions
  newfunction(:resolve_cli_filename, :type => :rvalue) do |args|

    def extract_version(filename)
      m = /\w+-(\d+(?:\.\d+)*)(.*)\.egg/.match(filename)
      version_numbers = m[1].split('.').map { |s| s.to_i }
      version_tag = m[2]
      version_numbers << version_tag
    end

    def get_listing(repository)
      unless repository.end_with? '/'
        repository = repository + '/'
      end
      url = URI.parse(repository)
      res = Net::HTTP.start(url.host, url.port) { |http|
        req = Net::HTTP::Get.new(url.path)
        req.basic_auth url.user, url.password
        http.request(req)
      }
      res.body
    end

    def get_latest_version(listing)
      listing.scan(/<a href="[^"]*">(.*?)<\/a>/).map { |groups|
        groups[0]
      }.sort_by{ |f|
        extract_version(f)
      }.last
    end

    def resolve_cli_filename(repository, version)
      if version == 'latest'
        return get_latest_version(get_listing(repository))
      else
        return "cosmos-#{version}.egg"
      end
    end

    resolve_cli_filename(args[0], args[1])
  end
end
