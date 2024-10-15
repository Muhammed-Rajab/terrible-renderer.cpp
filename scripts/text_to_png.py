import os
import sys
import subprocess
from concurrent.futures import ThreadPoolExecutor, as_completed

CAPTURES_FOLDER = "./captures/"
RENDERS_FOLDER = "./renders/"

files = os.listdir(CAPTURES_FOLDER)
files = sorted([int(file) for file in files if os.path.isfile(os.path.join(CAPTURES_FOLDER, file))])

def render_file_to_png(filename: str):
    command = ["freeze", os.path.join(CAPTURES_FOLDER, str(filename)), "-m", "0",  "-p", "0","-o", os.path.join(RENDERS_FOLDER, f"{filename}.png")]
    results = subprocess.run(command, capture_output=True, text=True)
    print(results.stdout)
    if (results.stderr != ""):
        raise Exception("got stderr: ", results.stderr)

MAX_WORKERS = 8

with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:

    future_to_file = {executor.submit(render_file_to_png, file):file for file in files}

    for future in as_completed(future_to_file):

        file = future_to_file[future]

        try:
            output = future.result()
            print(f"processed {file}:  {output}")
        except Exception as exc:
            print(f"file {file} generated an exception: {exc}")