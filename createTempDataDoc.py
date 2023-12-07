'''
Created by Phil Brehart
This program takes feed data from the temperature sensor and produces a document showing the
average, minimum and maximum values. 

Example Invocation: python3 createTempDataDoc.py feed.csv
'''

import sys
import csv

infile = open(sys.argv[1], 'r')  # opens csv file holding feed data
reader = csv.reader(infile)  # The file object is converted to a csv.reader object.

headings = next(reader)  # Use next() to continue through file

rows = [] # list for storing data

# Iterate through remaining rows.
for line in reader:
    rows.append(float(line[1]))

print(f"Unsorted array: {rows}")
infile.close()  # Close the file when done

# Bubble sort to sort through list elements
for i in range(len(rows)-1, 0, -1):
    for j in range(i):
        if rows[j] > rows[j + 1]:
            swapped = True
            # swap data if the current element is less than the next element
            rows[j], rows[j + 1] = rows[j + 1], rows[j]

print("\n")
print(f"Sorted array: {rows}")

# obtain min max and average
minVal = rows[0]
maxVal = rows[-1]
curSum = 0

# get value for total sum
for i in range(len(rows)):
    curSum += rows[i]

avgVal = curSum / len(rows)

print("\n")
print(f"Average: {avgVal} Minimum: {minVal} Maximum: {maxVal}")

# write data to a txt file
newFile = open("ProcessedFeedData.txt", "w")

newFile.write("Sorted Array: \n")

for i in range(len(rows)):
    newFile.write(str(rows[i]))
    newFile.write(' ')
    if (i + 1) % 5 == 0:
        newFile.write('\n') # keep contents organized in rows of 5

newFile.write(f"\nAverage: {avgVal} Minimum: {minVal} Maximum: {maxVal}")
print("Data stored in file: ProcessedFeedData.txt")
newFile.close()
