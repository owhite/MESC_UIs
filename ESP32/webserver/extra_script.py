Import("env")

# Do not run a script when external applications, such as IDEs,
# dump integration data. Otherwise, input() will block the process
# waiting for the user input

env.Execute("python generate_header.py")

