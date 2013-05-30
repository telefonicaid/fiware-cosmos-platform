/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

--
-- Initial data for table Machine
--

LOCK TABLES `Machine` WRITE;
/*!40000 ALTER TABLE `Machine` DISABLE KEYS */;
INSERT INTO `Machine` VALUES
    ('cosmos02','cosmos02.hi.inet','ce68b0ed-7428-401c-9fb5-afcb9734ffac',1,2,'10.95.106.182',2,0),
    ('cosmos03','cosmos03.hi.inet','d984f581-9fb1-4553-95c5-3e51056ef7fa',2,3,'10.95.105.184',2,1),
    ('cosmos04','cosmos04.hi.inet','6ca551cf-05e5-482c-8ced-e64bfe7921bf',3,3,'10.95.106.179',2,1),
    ('cosmos05','cosmos05.hi.inet','d7950ab4-f580-48b7-b047-dd46a990e4d6',4,4,'10.95.106.184',2,1),
    ('cosmos06','cosmos06.hi.inet','2377249f-e229-4a8f-be1f-509c014c893b',5,5,'10.95.110.99',2,1),
    ('cosmos07','cosmos07.hi.inet','ee96da5b-523f-438c-acaf-c4aedac661bc',6,2,'10.95.110.203',2,1),
    ('cosmos08','cosmos08.hi.inet','c95f8a4c-408f-477d-b2f3-38c93d979e2e',7,3,'10.95.111.160',2,1),
    ('cosmos09','cosmos09.hi.inet','f5858d46-b604-41ff-9a6d-6ad039176afc',8,4,'10.95.108.75',2,1);
/*!40000 ALTER TABLE `Machine` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;
