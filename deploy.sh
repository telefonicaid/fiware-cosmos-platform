#!/bin/bash

if [ $# -ne 1 ]; then
	echo "[ERROR] Incorrect number of params"
	echo "[DEBUG] Need to pass in configuration file"
	exit 1
fi

echo "[DEBUG] Preparing to deploy"
echo "[DEBUG] Reading config file $1..."
source $1
echo "[DEBUG]     Frontend   = $frontend"
echo "[DEBUG]     Backend    = $backend"
echo "[DEBUG]     Login user = $login_user"
echo "[DEBUG]     Login pass = $login_pass"

echo "[DEBUG] Deploying frontend..."
# TODO: Deploy frontend

echo "[DEBUG] Deploying backend..."
# TODO: Deploy backend