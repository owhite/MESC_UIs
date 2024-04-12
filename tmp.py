#!/usr/bin/env python3

def split_by_block(text):
    # Find the index of the first occurrence of "BLOCK>"
    index = text.find("BLOCK>")

    if index != -1:
        before_block = text[:index].strip()
        index += len("BLOCK>")
        after_block = text[index:].strip()

        return before_block, after_block

    return None, text

text1 = """
thing
BLOCK>get
Text to keep here {12 12: 200202: A BB CC} {12 12: 200202: A BB CC}
{lslsl 12 12: 200202: A BB CC} more text to keep here
I want this
I want this also
Vd	1.964988	-4096.00	4096.00	FOC_Vdq_d
Vq	0.623176	-4096.00	4096.00	FOC_Vdq_q

BLOCK>status json

BLOCK>
ending"""

text2 = """
BLOCK>get
Text to keep here {12 12: 200202: A BB CC} {12 12: 200202: A BB CC}
{lslsl 12 12: 200202: A BB CC} more text to keep here
I want this
I want this also
Vd	1.964988	-4096.00	4096.00	FOC_Vdq_d
Vq	0.623176	-4096.00	4096.00	FOC_Vdq_q

BLOCK>status json

BLOCK>
ending"""


# Call split_by_block initially
before_block, after_block = split_by_block(text1)
while before_block is not None:
    if len(before_block) != 0:
           print("BLOCK>", before_block)
    print("-----")
    before_block, after_block = split_by_block(after_block)

print(">BLOCK", after_block.strip())

print("-----------------------")

# Call split_by_block initially
before_block, after_block = split_by_block(text2)
while before_block is not None:
    if len(before_block) != 0:
           print("BLOCK>", before_block)
    print("-----")
    before_block, after_block = split_by_block(after_block)

print(">BLOCK", after_block.strip())
