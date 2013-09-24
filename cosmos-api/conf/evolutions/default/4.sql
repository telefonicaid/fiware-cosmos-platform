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

-- * Add creation timestamp to clusters

# --- !Ups
ALTER TABLE `cluster` ADD `creation_date` TIMESTAMP NOT NULL DEFAULT now();
UPDATE `cluster` SET `creation_date` = now();

# --- !Downs
ALTER TABLE `cluster` DROP COLUMN `creation_date`;
