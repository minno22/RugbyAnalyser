import Log_Match

import requests
from bs4 import BeautifulSoup

def GetLinks(url):  
  #url = "https://www.ultimaterugby.com/match/list?date=recent"
  
  r = requests.get(url)
  #print (r.content)
  
  soup = BeautifulSoup(r.content, "html.parser")
  #print (soup.prettify())
  
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
                    Log_Match.Connect2Web(l)

			
def IterateThroughPages():  
  url = "https://www.ultimaterugby.com/match/list?date=recent"
  for x in range(0, 24):
    GetLinks(url)
    url = getNextPage(url)
  
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