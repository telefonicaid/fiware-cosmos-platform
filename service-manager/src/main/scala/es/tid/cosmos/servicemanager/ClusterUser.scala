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

package es.tid.cosmos.servicemanager

/**
 * Representation of a cluster user.
 *
 * @param userName the user's username
 * @param publicKey the user's public key that will be used to grant the user access to the cluster
 * @param sshEnabled whether the user is allowed to SSH the cluster
 * @param hdfsEnabled whether the user is allowed to access cluster HDFS and have a home folder
 */
case class ClusterUser(
    userName: String,
    publicKey: String,
    sshEnabled: Boolean = true,
    hdfsEnabled: Boolean = true,
    isSudoer: Boolean = false)
