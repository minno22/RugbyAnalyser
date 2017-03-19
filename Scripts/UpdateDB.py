import Log_Match

import requests
from bs4 import BeautifulSoup
import pyodbc
import re
from enum import Enum

def GetLinks(url, date):  
  #url = "https://www.ultimaterugby.com/match/list?date=recent"
  
  r = requests.get(url)
  #print (r.content)
  
  soup = BeautifulSoup(r.content, "html.parser")
  #print (soup.prettify())
  
  bool = 0
  
  tabs = soup.find_all("div", {"class": "match-list"})
  for t in tabs:
    #if "Results" in t.text:
      matches = t.find_all("div", {"class": "match-item"})
      for match in matches:
        if "TBC" not in match.text:
          if "7's" not in match.text:
            if ":" not in match.text:
              #if "Women" not in match.text:
                links = match.find_all("a")
                for link in links:
                  if "/match/" in link.get("href"):
                    l = "https://www.ultimaterugby.com%s/commentary" % link.get("href")
                    bool = CheckDate(l, date)
                    if bool == 1:
                      Log_Match.Connect2Web(l)
                      #print ("DEBUG VAlID MATCH")
  return bool

def CheckDate(url, oldDate):
  r = requests.get(url)
  soup = BeautifulSoup(r.content, "html.parser")

  match_data = soup.find_all("div", {"class": "block-summary match-summary"})
  for match in match_data:
    
    date = match.find_all("div", {"class": "status-ko"})
    for d in date:
      dateString = d.text.strip()
	  
    Month = Enum('Month', 'Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec')
    nums = re.findall('\d+', dateString)
	
    strs = re.findall('[a-zA-Z]+', dateString)
    for s in strs:
      mon = s
  
    date = "%s-%d-%s" % (nums[1], Month[mon].value, nums[0])
    #print (date)
	
    dateNums = re.findall('\d+', str(oldDate))
	
    if int(nums[1]) < int(dateNums[0]):
      return 0
      #print ("DEBUG INVALID")
    elif int(nums[1]) == int(dateNums[0]):
      if Month[mon].value < int(dateNums[1]):
        return 0
        #print ("DEBUG INVALID")
      elif Month[mon].value == int(dateNums[1]):
        if int(nums[0]) <= int(dateNums[2]):
          return 0
          #print ("DEBUG INVALID")
		
    #print ("DEBUG VALID")
    return 1

					
def IterateThroughPages():  
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()
	
  cursor.execute("SELECT MAX(MatchDate) FROM Match")
  for row in cursor:
    date = row[0]
	
  #print (date)
  
  url = "https://www.ultimaterugby.com/match/list?date=recent"
  
  bool = 1
  while bool == 1:
  #for x in range(0, 24):
    bool = GetLinks(url, date)
    url = getNextPage(url)
    print (bool)
  
def getNextPage(url):
  r = requests.get(url)
  #print (r.content)
  
  soup = BeautifulSoup(r.content, "html.parser")
  #print (soup.prettify())
  
  div = soup.find_all("li", {"class": "next"})
  for d in div:
    links = d.find_all("a")
    for link in links:
      next = "https://www.ultimaterugby.com%s" % link.get("href")
      return next
			
#Define a main() function that prints a litte greeting
def main():
  #GetLinks()
  IterateThroughPages()

# This is the standard boilerplate that calls the maun function.
if __name__ == '__main__':
    main()