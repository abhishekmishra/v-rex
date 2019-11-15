import csv
import os

currentpath = os.path.dirname(os.path.realpath(__file__))

with open(os.path.join(currentpath,'WebColours.csv'), newline='\n') as csvfile:
    # write defines
    colours_reader = csv.reader(csvfile, delimiter=',')
    count = 0
    for row in colours_reader:
        if count != 0:
            print('#define VREX_{} wxT("VREX_{}")'.format(row[0].upper(), row[0].upper()))
        count += 1

with open(os.path.join(currentpath,'WebColours.csv'), newline='\n') as csvfile:
    # write colour declarations
    colours_reader = csv.reader(csvfile, delimiter=',')
    count = 0
    for row in colours_reader:
        if count != 0:
            RGBint = int(row[1].replace('#', '0x'), 16)
            Blue =  RGBint & 255
            Green = (RGBint >> 8) & 255
            Red =   (RGBint >> 16) & 255
            print('wxTheColourDatabase->AddColour(VREX_{}, wxColour({}, {}, {})); //{}'.format(row[0].upper(), Red, Green, Blue, row[1]));
        count += 1
