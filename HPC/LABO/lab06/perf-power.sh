#!/bin/bash

# Function to extract data
extract_data() {
    echo "$1" | awk '/power\/energy-cores/ {print $1}' # Cores Energy
    echo "$1" | awk '/power\/energy-gpu/ {print $1}' # GPU Energy
    echo "$1" | awk '/power\/energy-pkg/ {print $1}' # PKG Energy
    echo "$1" | awk '/power\/energy-psys/ {print $1}' # PSYS Energy
    echo "$1" | awk '/power\/energy-ram/ {print $1}' # RAM Energy
}

# Run non-vectorized version
NON_VEC_OUTPUT=$(sudo perf stat -e power/energy-cores/ -e power/energy-gpu/ -e power/energy-pkg/ -e power/energy-psys/ -e power/energy-ram/ ./lab06 2>&1)

# Run vectorized version
VEC_OUTPUT=$(sudo perf stat -e power/energy-cores/ -e power/energy-gpu/ -e power/energy-pkg/ -e power/energy-psys/ -e power/energy-ram/ ./lab06_vec 2>&1)

# Extract data
NON_VEC_DATA=$(extract_data "$NON_VEC_OUTPUT")
VEC_DATA=$(extract_data "$VEC_OUTPUT")

# Print in markdown table format
echo "| Mesures | Non-Vectoriser | Vectoriser |"
echo "|-------------|----------------|------------|"
paste -d'|' \
<(echo -e "|Cores (Joules)\n|GPU (Joules)\n|PKG (Joules)\n|PSYS (Joules)\n|RAM (Joules)") \
<(echo -e "$NON_VEC_DATA") \
<(echo -e "$VEC_DATA") \
<(echo -e "\n\n\n\n")