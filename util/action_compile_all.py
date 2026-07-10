import os
import subprocess
import time
import shutil
import sys

if len(sys.argv) <= 1:
    print("Usage: python action_compile_all.py <path_to_example_folder>")
    sys.exit(1)

example_folder = sys.argv[1]
if not os.path.isdir(example_folder):
    print(f"example search directory not found at {example_folder}")
    exit(1)

arduino_package_path = ""
if len(sys.argv) >= 3:
    # Second argument is config dir (newer CLI) or config file path (1.0.3).
    config_path = sys.argv[2]
    if config_path.endswith(".yaml"):
        arduino_package_path = "--config-file " + config_path
    else:
        arduino_package_path = "--config-dir " + config_path

compiled_hex_folder = os.path.join(example_folder, "compiled_hex")
if not os.path.isdir(compiled_hex_folder):
    os.makedirs(compiled_hex_folder)

#iterate every sub folder in the example_folder
example_directories = []
for subdir, dirs, files in os.walk(example_folder):
    dirs.sort()
    for dir in dirs:
        potential_example_directory = os.path.join(subdir, dir)
        example_ino_name = dir + ".ino"
        example_ino_full_path = os.path.join(potential_example_directory, example_ino_name)
        if os.path.isfile(example_ino_full_path):
            example_directories.append(potential_example_directory)
            #print(f"Found example at {potential_example_directory}")

build_successful = True
error_log_string = ""
for example_directory in example_directories:
    board_options_string = ""
    sketch_file = os.path.join(example_directory, os.path.basename(example_directory)+".ino")
    #check if there is a line in the sketch file that contains "cli board options:"
    if os.path.isfile(sketch_file):
        with open(sketch_file, 'r') as fp:
            lines = fp.readlines()
            for row in lines:
                if row.find('cli board options:') != -1:
                    board_options_string = '--board-options '+row.replace('cli board options:', '').strip()
                    break

    build_cmd = f"arduino-cli compile {arduino_package_path} --fqbn CH55xDuino:mcs51:ch552 --output-dir {compiled_hex_folder} {example_directory}"
    #if there are two consecutive spaces, remove them
    build_cmd = build_cmd.replace("  ", " ")
    if board_options_string != "":
        build_cmd = build_cmd + " " + board_options_string
    example_name = os.path.basename(example_directory)
    print(f"Building {example_name} with command: {build_cmd}")
    build_start_time = time.monotonic()
    build_process = subprocess.Popen(build_cmd.split(" "), stdin=None, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = build_process.communicate() 
    build_end_time = time.monotonic()
    build_time = build_end_time - build_start_time
    return_code = build_process.wait()
    if return_code == 0:
        print(f"Build of {example_name} completed in {build_time:.2f} seconds")
    else:
        #append output to error log file
        err_string = err.decode('utf-8')
        print(f"Error building {example_name}")
        error_log_string += f"Error building {example_name}\n"
        error_log_string += err_string + "\n"
        print(err_string)
        build_successful = False

if build_successful:
    print("All examples built successfully")
else:
    print("One or more examples failed to build")
    print("================================================")
    print("Error log:")
    print(error_log_string)
    sys.exit(1)
