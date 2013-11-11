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

-- * Add email as 254-chars varchar field (defined at RFC 3696) to user table

# --- !Ups
ALTER TABLE `user` ADD `email` VARCHAR(254) NOT NULL;
UPDATE `user` SET `email` = 'no-reply@example.com';

# --- !Downs
ALTER TABLE `user` DROP COLUMN `email`;
