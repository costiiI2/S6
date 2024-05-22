#!/bin/bash

# Run non-vectorized version
NON_VEC_OUTPUT=$(sudo likwid-powermeter ./lab06)

# Run vectorized version
VEC_OUTPUT=$(sudo likwid-powermeter ./lab06_vec)

# Function to extract data
extract_data() {
    echo "$1" | awk '/Runtime:/ {print $2}' # Runtime
    echo "$1" | awk '/Domain PKG:/,/Domain PP0:/ {if ($1=="Energy") print $3}' # PKG Energy
    echo "$1" | awk '/Domain PKG:/,/Domain PP0:/ {if ($1=="Power") print $3}' # PKG Power
    echo "$1" | awk '/Domain PP0:/,/Domain PP1:/ {if ($1=="Energy") print $3}' # PP0 Energy
    echo "$1" | awk '/Domain PP0:/,/Domain PP1:/ {if ($1=="Power") print $3}' # PP0 Power
    echo "$1" | awk '/Domain PLATFORM:/,/-----/ {if ($1=="Energy") print $3}' # PLATFORM Energy
    echo "$1" | awk '/Domain PLATFORM:/,/-----/ {if ($1=="Power") print $3}' # PLATFORM Power
    echo "$1" | awk '/Domain DRAM:/,/Domain PLATFORM:/ {if ($1=="Energy") print $3}' # DRAM Energy
    echo "$1" | awk '/Domain DRAM:/,/Domain PLATFORM:/ {if ($1=="Power") print $3}' # DRAM Power
    echo "$1" | awk '/Domain PLATFORM:/,/-----/ {if ($1=="Energy") print $3}' # PLATFORM Energy
echo "$1" | awk '/Domain PLATFORM:/,/-----/ {if ($1=="Power") print $3}' # PLATFORM Power
}

# Extract data
NON_VEC_DATA=$(extract_data "$NON_VEC_OUTPUT")
VEC_DATA=$(extract_data "$VEC_OUTPUT")

# Print in markdown table format
echo "| Mesures | Non-Vectoriser | Vectoriser |"
echo "|-------------|----------------|------------|"
paste -d'|' \
<(echo -e "|Runtime (s)\n|PKG (Joules)\n|PKG (Watt)\n|PP0 (Joules)\n|PP0 (Watt)\n|PP1 (Joules)\n|PP1 (Watt)\n|DRAM (Joules)\n|DRAM (Watt)\n|PLATFORM (Joules)\n|PLATFORM (Watt)") \
<(echo -e "$NON_VEC_DATA") \
<(echo -e "$VEC_DATA") \
<(echo -e "\n\n\n\n\n\n\n\n\n\n") 