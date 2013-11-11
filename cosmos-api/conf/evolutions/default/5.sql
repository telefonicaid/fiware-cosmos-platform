--
-- Telefónica Digital - Product Development and Innovation
--
-- THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
-- EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
--
-- Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
-- All rights reserved.
--

-- * Add user state field to user table

# --- !Ups
ALTER TABLE `user` ADD `state` VARCHAR(10) NOT NULL DEFAULT 'enabled';

# --- !Downs
ALTER TABLE `user` DROP COLUMN `state`;
