# Licensed to Cloudera, Inc. under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  Cloudera, Inc. licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from desktop.lib.windmill_util import logged_in_client

def test_cosmos_dataviewer():
  """ launches the default view for cosmos_dataviewer """
  client = logged_in_client()
  client.click(id='hue-cosmos_dataviewer-menu')
  client.waits.forElement(classname='Hue-COSMOS_DATAVIEWER', timeout='2000')  
