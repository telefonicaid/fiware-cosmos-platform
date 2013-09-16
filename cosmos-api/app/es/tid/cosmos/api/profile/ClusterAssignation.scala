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

package es.tid.cosmos.api.profile

import java.util.Date

import es.tid.cosmos.servicemanager.ClusterId

/**
 * Association between a cluster and his owner.
 *
 * @param clusterId    Owned cluster
 * @param ownerId        Cluster creator
 * @param creationDate Creation timestamp
 */
case class ClusterAssignation(clusterId: ClusterId, ownerId: Long, creationDate: Date)
