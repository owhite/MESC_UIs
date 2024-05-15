#!/usr/bin/env python3

class MyClass:
    def __init__(self, variable=None):
        self.variable = variable

    def function1(self):
        print(f"This is function1 with variable: {self.variable}")

    def function2(self):
        print(f"This is function2 with variable: {self.variable}")

    def function3(self):
        print(f"This is function3 with variable: {self.variable}")

    def function4(self):
        print(f"This is function4 with variable: {self.variable}")

# Example variable containing the function name
function_name = "function2"  # Change this to test different function names

# Pass a variable to MyClass upon instantiation
my_variable = "some_value"
my_instance = MyClass(variable=my_variable)

try:
    # Dynamically get the method object using getattr
    method_to_call = getattr(my_instance, function_name)
    
    # Call the method
    method_to_call()
except AttributeError:
    print(f"Function '{function_name}' does not exist.")
