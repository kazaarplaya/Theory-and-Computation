# script to create a large file with valid input
with open("large_valid_test.txt", "w") as f:
    for i in range(100000):
        f.write(f"int variable_{i} = {i};\n")
        f.write(f"char name_{i};\n")
        f.write(f"if (variable_{i}) {{ return {i}; }}\n")

# script to create a large file with error tokens
with open("large_error_test.txt", "w") as f:
    for i in range(100000):
        f.write(f"int badNumber_{i} = 123abc;\n")
        f.write(f"name@value_{i};\n")
        f.write("@@@###$$$\n")