import subprocess
import os

file_paths = []

for (root, dirs, files) in os.walk("."):
    # print("Walking:", root, "Files:", files)
    for file_name in files:
        if not (file_name.endswith(".c") or file_name.endswith(".h") \
                or file_name.endswith(".cpp") or file_name.endswith(".hpp")):
            continue
        code_file_path = f"{root}/{file_name}"
        file_paths.append(code_file_path)

for file_path in file_paths:
    command = f"clang-format -i {file_path}"
    print(command)
    subprocess.run(command, shell=True)
