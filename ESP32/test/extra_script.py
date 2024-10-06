import os
Import("env")

def before_build(source, target, env):
    # Build the LittleFS filesystem image
    env.Execute("pio run --target buildfs")

def before_upload(source, target, env):
    # Upload the LittleFS filesystem image
    env.Execute("pio run --target uploadfs")

env.AddPreAction("upload", before_build)
env.AddPreAction("upload", before_upload)

