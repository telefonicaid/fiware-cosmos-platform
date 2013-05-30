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
-- Table structure for table `SERVERS`
--

DROP TABLE IF EXISTS `SERVERS`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `SERVERS` (
  `name` varchar(128) NOT NULL,
  `numberOfCpus` int(11) NOT NULL,
  `bridgeName` varchar(128) NOT NULL,
  `description` varchar(128) NOT NULL,
  `enabled` tinyint(1) NOT NULL,
  `domainHostname` varchar(128) NOT NULL,
  `domainTemplate` varchar(128) NOT NULL,
  `connectionChain` varchar(128) NOT NULL,
  `id` bigint(20) NOT NULL AUTO_INCREMENT,
  `profile` int(11) NOT NULL,
  `totalMemory` int(11) NOT NULL,
  `domainIpAddress` varchar(128) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=6 DEFAULT CHARSET=latin1;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `SERVERS`
--

LOCK TABLES `SERVERS` WRITE;
/*!40000 ALTER TABLE `SERVERS` DISABLE KEYS */;
INSERT INTO `SERVERS` VALUES
    (
        'andromeda02',
        4,
        'vzbr0',
        'Andromeda 02',
        1,
        'andromeda52',
        'centos-6-x86_64',
        'openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock',
        1,
        3,
        32768,
        '192.168.63.62'
    ),
    (
        'andromeda03',
        4,
        'vzbr0',
        'Andromeda 03',
        1,
        'andromeda53',
        'centos-6-x86_64',
        'openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock',
        2,
        3,
        32768,
        '192.168.63.63'
    ),
    (
        'andromeda04',
        4,
        'vzbr0',
        'Andromeda 04',
        1,
        'andromeda54',
        'centos-6-x86_64',
        'openvz+ssh://cosmos@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock',
        3,
        3,
        32768,
        '192.168.63.64'
    ),
    (
        'andromeda05',
        4,
        'vzbr0',
        'Andromeda 05',
        1,
        'andromeda55',
        'centos-6-x86_64',
        'openvz+ssh://cosmos@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock',
        4,
        3,
        32768,
        '192.168.63.65'
    ),
    (
        'andromeda06',
        4,
        'vzbr0',
        'Andromeda 06',
        1,
        'andromeda56',
        'centos-6-x86_64',
        'openvz+ssh://cosmos@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock',
        5,
        3,
        32768,
        '192.168.63.66'
    );

/*!40000 ALTER TABLE `SERVERS` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;