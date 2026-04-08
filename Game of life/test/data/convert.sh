#!/bin/bash

# Check if both input and output files are provided
if [ $# -ne 2 ]; then
    echo "Usage: $0 <input_file> <output_file>"
    exit 1
fi

input_file="$1"
output_file="$2"

# Check if input file exists
if [ ! -f "$input_file" ]; then
    echo "Error: File '$input_file' not found."
    exit 1
fi

# Process each line in the input file
while IFS= read -r line; do
    # Replace . with -
    converted_line="${line//./-}"

    # Convert O to o
    converted_line="${converted_line//O/o}"

    # Add spaces between characters
    formatted_line=$(echo "$converted_line" | sed 's/./& /g')

    # Remove the last space
    formatted_line=$(echo "$formatted_line" | sed 's/ $//')

    # Append the formatted line to the output file
    echo -n "$formatted_line" >> "$output_file"

    # Add newline only if not the last line
    if [[ ! -z "${line// }" ]]; then
        echo "" >> "$output_file"
    fi
done < "$input_file"

echo "Conversion completed. Output saved to $output_file."
