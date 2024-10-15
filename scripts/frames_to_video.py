import os
import sys
import subprocess

RENDERS_FOLDER = "./renders/"

def render_frames_to_video(filepath: str):
    command =[
        "ffmpeg",
        "-framerate", "60",
        "-i", "./renders/%d.png",
        "-vf", "scale=iw*0.5:ih*0.5",
        "-c:v", "libx264",
        "-crf", "18",
        "-preset", "slow",
        "-pix_fmt", "yuv420p",
        filepath
    ]

    with subprocess.Popen(command, stdout=subprocess.PIPE, stderr= subprocess.STDOUT, text=True) as proc:
        for line in proc.stdout:
            print(line, end="")


filepath = sys.argv[1]
render_frames_to_video(filepath)