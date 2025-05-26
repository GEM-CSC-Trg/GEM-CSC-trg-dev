#### this is for comparing two separate results, given a file/bunch of files,
#### The eventual cuts to apply can be put in a specific config file in your area
#### thus you need to set the config file path in the script below
#### the comparison can be event-by-event or file-based for the moment
import argparse
import os
import sys
import yaml

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compare two sets of results.")
    parser.add_argument(
        "--config",
        type=str,
        required=True,
        help="Path to the configuration file containing comparison settings.",
    )
    parser.add_argument(
        "--file1",
        type=str,
        required=True,
        help="Path to the first result file or directory.",
    )
    parser.add_argument(
        "--file2",
        type=str,
        required=True,
        help="Path to the second result file or directory.",
    )

    args = parser.parse_args()

    # Load configuration
    with open(args.config, 'r') as config_file:
        config = yaml.safe_load(config_file)

    # Implement comparison logic here
    print(f"Comparing {args.file1} with {args.file2} using configuration: {config}")