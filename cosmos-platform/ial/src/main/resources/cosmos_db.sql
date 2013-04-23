-- MySQL dump 10.13  Distrib 5.6.10, for osx10.8 (x86_64)
--
-- Host: localhost    Database: test_33297225
-- ------------------------------------------------------
-- Server version	5.6.10

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Machine`
--

DROP TABLE IF EXISTS `Machine`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Machine` (
  `name` varchar(128) NOT NULL,
  `hostname` varchar(128) NOT NULL,
  `machineId` char(36) NOT NULL,
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `profile` int(11) NOT NULL,
  `ipAddress` varchar(128) NOT NULL,
  `status` int(11) NOT NULL,
  `available` tinyint(1) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=9 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Machine`
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

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2013-04-23 15:22:10
