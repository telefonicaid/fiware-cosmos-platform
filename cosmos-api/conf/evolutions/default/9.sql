--
-- Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--

-- * Add shared and secret fields to cluster table

# --- !Ups

ALTER TABLE cluster ADD COLUMN shared TINYINT NOT NULL DEFAULT 0;
ALTER TABLE cluster ADD COLUMN cluster_secret VARCHAR(128);

# --- !Downs

ALTER TABLE cluster DROP COLUMN shared;
ALTER TABLE cluster DROP COLUMN cluster_secret;
