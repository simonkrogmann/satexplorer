import sys
import os

def ensure_dir_exists(name):
    directory = os.path.dirname(name)
    if directory and not os.path.isdir(directory):
        os.makedirs(directory)

def get_file_conversion_arguments(input_description, output_description):
    if len(sys.argv) < 3:
        print(f'Usage: {sys.argv[0]} </path/to/{input_description}> </path/to/{output_description}>')
        exit(1)
    ensure_dir_exists(sys.argv[2])
    return sys.argv[1], sys.argv[2]
