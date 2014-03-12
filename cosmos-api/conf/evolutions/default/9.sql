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

-- * Add shared and secret fields to cluster table

# --- !Ups

ALTER TABLE cluster ADD COLUMN shared TINYINT NOT NULL DEFAULT 0;
ALTER TABLE cluster ADD COLUMN cluster_secret VARCHAR(128);

# --- !Downs

ALTER TABLE cluster DROP COLUMN shared;
ALTER TABLE cluster DROP COLUMN cluster_secret;
