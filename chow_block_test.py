#!/usr/bin/env python3

#!/usr/bin/env python3

text1 = "usb@MESC>0000000000usb@MESC>111111111111usb@MESC>22222222222usb@MESC>33333333333" # remove token, get payload, remainder, continue
text2 = "usb@MESC>33333333333"                                           # remove token,  no payload, return input, stop
text3 = "usb@MESC>"                                                                    #  no payload, return input, stop
text4 = "0000000000usb@MESC>111111111111usb@MESC>22222222222usb@MESC>33333333333"                  # get payload, remainder, continue
text5 = "0000000000usb@MESC>"                                                          # get payload, remainder = token, stop
text6 = "333333"                                                                 #  no payload, return input, stop
text7 = ""                                                                       #  no payload, return input, stop

text8 = """
su -g
 Godmode
usb@MESC>status stop

usb@MESC>get
Parameter Value Min Max Description
adc1 3996 0 4096 Raw ADC throttle
adc1_max 2700 0 4096 ADC1 max val
adc1_min 800 0 4096 ADC1 min val
adc1_pol 1.000000 -1.00 1.00 ADC1 polarity
adc2_max 4095 0 4096 ADC2 max val

usb@MESC>status json

usb@MESC>

"""
text9 = """usb@MESC>get
Parameter Value Min Max Description
adc1 3996 0 4096 Raw ADC throttle
adc1_max 2700 0 4096 ADC1 max val
adc1_min 800 0 4096 ADC1 min val
adc1_pol 1.000000 -1.00 1.00 ADC1 polarity
adc2_max 4095 0 4096 ADC2 max val

usb@MESC>status json

usb@MESC>

"""
l = [text1, text2, text3, text4, text5, text6, text7, text8, text9]

# This funtion tests for MESC> prompt in a string.
#   Payload chunks are found BEFORE the prompt, extracted. Material
#   after the prompt is saved until another prompt appears
# Try the MESC terminal and you'll see why.
#
# RETURNS:
# 'flag' set to False to break out of while loop if its in one
#  'payload' text from MESC stream appearing before prompt
#  'remainder' either gets saved in a buffer for later
#   or continues to get processed if this function
#   is used in a while loop

def chow_block(text, token):
    flag = False

    t = text
    # not really intuitive but remove the token if at beginning of string
    #  the idea is the remainder will get save for the next round of parsing
    if text.startswith(token):
        t = text.replace(token, '', 1)

    if len(t) == 0: # you get this if string was empty or was just a prompt
        return False, t, t

    l = t.split(token)
    payload = ''
    remainder = ''
    if len(l) == 1: # no token, everything goes in remainder
        remainder = l[0]
    elif len(l) == 2:
        payload = l[0]
    elif len(l) > 2:
        flag = True
        payload = l[0]
        remainder = token.join(l[1:])
    else:
        print("chow_block: unforseen parsing condition")
        payload = "chow_block: unforseen parsing condition"

    # fakes the user out by showing the prompt at the beginning of the payload
    if len(payload) > 0:
        payload = token + payload

    return flag, payload, remainder

token = "usb@MESC>"

count = 1
for string in l:
    print("="* 40)
    print("TEST", count, string)
    count += 1
    result, first_block, second_block = chow_block(string, token)
    print(result)
    print("1: " + first_block)
    print("2: " + second_block)
    print("="* 40)

print("="* 40)
print("TEST", text1)

result, first_block, second_block = chow_block(text8, token)
while(result):    
    print("1: " + first_block)
    print("2: " + second_block)
    result, first_block, second_block = chow_block(second_block, token)

print("1: " + first_block)
print("2: " + second_block)
