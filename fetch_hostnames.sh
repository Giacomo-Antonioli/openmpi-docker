#!/bin/bash

# Define the target hostfile
HOSTFILE="mpi_hostname"

# Overwrite/create the hostfile
> "$HOSTFILE"

# List of node hostnames to resolve
NODE_HOSTNAMES=("node1" "node2" "node3" "node4")

echo "Resolving node IPs..."

# Loop through hostnames and get their IPs
for node_name in "${NODE_HOSTNAMES[@]}"; do
    # Use dig to get the IP address. +short provides just the IP.
    # Removed incompatible -T and -R flags. Added query type A.
    ip_address=$(dig +short "$node_name" A)

    if [ -n "$ip_address" ]; then
        # Make sure we only take the first IP if multiple are returned (unlikely here)
        ip_address=$(echo "$ip_address" | head -n 1)
        echo "$ip_address" >> "$HOSTFILE"
        echo " - Resolved $node_name to $ip_address"
    else
        echo " - Failed to resolve $node_name (Check if container is running and hostname is correct)" >&2 # Print error to stderr
    fi
done

echo "IP addresses configured in $HOSTFILE"
cat "$HOSTFILE" # Optional: show the created file
