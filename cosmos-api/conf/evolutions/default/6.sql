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

-- * Add email as 254-chars varchar field (defined at RFC 3696) to user table

# --- !Ups
ALTER TABLE `user` ADD `email` VARCHAR(254) NOT NULL DEFAULT 'no-reply@example.com';

# --- !Downs
ALTER TABLE `user` DROP COLUMN `email`;
