pushd $PWD

PARENT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." >/dev/null 2>&1 && pwd )"
cd $PARENT_DIR

vendor=$(lscpu | awk '/Vendor ID/{print $3}')
arch=$(lscpu | awk '/Architecture/{print $2}')
if [[ "$vendor" == "GenuineIntel" ]]; then
    cpu="intel"
elif [[ "$vendor" == "AuthenticAMD" ]]; then
    cpu="amd"
elif [[ "$arch" == "ppc64le" ]]; then
    cpu="power"
elif [[ "$arch" == "aarch64" ]]; then
    cpu="arm"
else
    echo "Unknown vendor: $vendor"
    echo "Unknown architecture: $arch"
    exit 1
fi

merge_files() {
    keyword="$1"
    output_file="${2}_${cpu}.csv"
    first_file=1
    first_file_header=""
    find . -name "*${keyword}*.csv" -type f | while read file
    do
        # If it's the first file, copy the whole file including header
        if [[ $first_file -eq 1 ]]
        then
            cat "$file" > "$output_file"
            first_file_header=$(head -n 1 "$file")
            first_file=0
        else
            # If it's not the first file, validate the header
            if [[ "$(head -n 1 "$file")" != "$first_file_header" ]]
            then
                echo "Error: File $file has a different header."
                rm "$output_file"
                exit 1
            fi
            # If the header is valid, append the content without header to the output file
            awk 'NR>1' "$file" >> "$output_file"
        fi
    done
    echo "$output_file"
}

reads_file=$(merge_files "finalreadresult" "reads")
writes_file=$(merge_files "finalwriteresult" "writes")

reads_url=$(curl --silent --upload-file "$reads_file" https://transfer.sh/"$reads_file")
writes_url=$(curl --silent --upload-file "$writes_file" https://transfer.sh/"$writes_file")

echo "Please download the read benchmark data from $reads_url and write benchmark data from $writes_url."

popd