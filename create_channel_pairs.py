#loop over channels
#get near and far tower, column of channel
#convert (near, far) tower, column to (near, far) channel
#create entry like: channel, near channel, far channel

import pandas as pd

#Read in csv file storing near and far column pairs
df = pd.read_csv("column_pairs.csv", names=['Tower', 'Column', 'Near_tower', 'Near_column', 'Far_tower', 'Far_column'])

#Get tower and column of channel of interest
for channel in range(1,999):
    if channel % 52 ==0:
        tower = channel//52
    else:
        tower = (channel//52) + 1
    if ((channel - (tower-1)*52)//13) ==0:
        column = ((channel - (tower-1)*52) // 13)
    else:
        column = ((channel - (tower-1)*52) // 13) + 1
    floor = channel % 13
    
#Filter entry in column_pairs with tower and column of interest
    df_search = df.query('Tower==tower & Column==column')
    near_tower = df_search.iloc[0]['Near_tower']
    near_column = df_search.iloc[0]['Near_column']
    far_tower = df_search.iloc[0]['Far_tower']
    far_column = df_search.iloc[0]['Far_column']

    near_channel = (52*(near_tower-1)) + (13*(near_column-1)) + floor
    far_channel = (52*(far_tower-1)) + (13*(far_column-1)) + floor
                   
#Print out channel, near_channel, far_channel in text file
    output_file = open("channel_pairs.txt","a")
    #output_file.write("%d,%d,%d \n", %(channel,near_channel,far_channel)) 

print("DONE")
