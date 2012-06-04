#!/usr/bin/env python
# Licensed to Cloudera, Inc. under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  Cloudera, Inc. licenses this file
# to you under the Apache License, Version 2.0 (the
# 'License'); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an 'AS IS' BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from django.conf.urls.defaults import patterns, url

urlpatterns = patterns('cosmos',
  url(r'^$',                   'views.index',          name='list_jobs'),
  url(r'^job/(\d+)/results/$', 'views.show_results',   name='show_results'),
  url(r'^jobs/run/$',          'views.define_job',     name='define_job'),
  url(r'^jobs/run/config/$',   'views.configure_job',  name='configure_job'),
  url(r'^jobs/run/confirm/$',  'views.confirm_job',    name='confirm_job'),
  url(r'^upload/$',            'views.upload_index',   name='upload_index'),
)
