#!/usr/bin/python3

import sys
import csv

#userId that we check previously
prev_userId = None
# Total rating counts of current user 
cumulative_count = 0
# user id of the user who provides the most ratings
max_userId = None
# Rating Count from the user who provides the most ratings
max_count = 0

#The genre name that the user providing most rating watches most
most_genre =None
#Count of the genre that the user providing most rating watches most
most_genre_count = 0

#Count of the genre that current user watches most
current_genre_count = 0
# the genre name that current user watches most
current_genre = None


for line in sys.stdin:
    userId, genre,genre_count = line.split("\t")
    genre_count = float(genre_count)
    # Find the genre that current user watches most
    if prev_userId== None or (prev_userId == userId and genre_count >current_genre_count):
        #if previous userId = None, just initialize current_genre_count
        # if previous userId == user Id,update the genre with maximum count of this user
        current_genre_count = genre_count
        current_genre = genre
        
    # Find the user that provides most rating
    if prev_userId and prev_userId != userId:
        # update user that provides most ratings
        if max_count < cumulative_count:
            max_count = cumulative_count
            max_userId = prev_userId
            most_genre = current_genre
            most_genre_count = current_genre_count
        # reset cumulative count of this user 
        # and start compute total count of the next user
        cumulative_count = 0
    #Update total count of rating of current user and userId
    cumulative_count += genre_count
    prev_userId = userId 
    
# check the last user information
if prev_userId and prev_userId != userId:
        # update user that provides most ratings
        if max_count < cumulative_count:
            max_count = cumulative_count
            max_userId = prev_userId
            most_genre = current_genre
            most_genre_count = current_genre_count
        cumulative_count = 0
    
print("User Identification:\n %s -- Total Rating Counts: %d -- Most Rated Genre:%s - %d"%(max_userId,max_count,most_genre, most_genre_count))
    
