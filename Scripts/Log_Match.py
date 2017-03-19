import requests
import pyodbc
import re

from bs4 import BeautifulSoup
from enum import Enum

def Connect2Web(url):
  #page = urllib.request.urlopen("http://www.ultimaterugby.com/match/argentina-vs-australia-at-twickenham-8th-oct-2016/11898/commentary");
  
  
  #page2 = str(BeautifulSoup(page))

  #print (page.read())
  
  #url = "http://www.ultimaterugby.com/match/ireland-vs-new-zealand-at-soldier-field-5th-nov-2016/11679/commentary"
  
  r = requests.get(url)
  #print (r.content)
  
  soup = BeautifulSoup(r.content, "html.parser")
  #print (soup.prettify())
  
  #links = soup.find_all("a")
  
  #for link in links:
   #   if "http" in link.get("href"):
	#      print (link.text, link.get("href"))
          #print ("<a href='%s'>s</a>" %(link.get("href"), link.text))

  match_data = soup.find_all("div", {"class": "block-summary match-summary"})
  for match in match_data:
    
    comp = match.find_all("a")  
    for c in comp:
      compString = c.text.strip()
      break
    
    home = match.find_all("div", {"class": "team-home"})
    for h in home:
      homeTeam = h.text.strip()
	  
    away = match.find_all("div", {"class": "team-away"})
    for a in away:
      awayTeam = a.text.strip()
	  
    date = match.find_all("div", {"class": "status-ko"})
    for d in date:
      dateString = d.text.strip()
	  
    scores = match.find_all("div", {"class": "score-cell"})
    homeScore = scores[0].text
    awayScore = scores[1].text

  print ("Logging Match:\t%32s%32s%32s%32s" %(compString, homeTeam, awayTeam, dateString)) 
    
  matchId = InsertMatchInfo(compString, homeTeam, awayTeam, dateString, homeScore, awayScore)

  strings=[]
  strings.append("event event-home")
  strings.append("event event-away")
		  
  conversion = 0
  team = 0
  for str in strings:
    g_data = soup.find_all("div", {"class": str})
    for item in g_data:
      t_data = item.find_all("div", {"class": "time"})
      for times in t_data:
          time = times.text
		  
      if "Penalty Try" in item.text:
          InsertTry(time, conversion, 1, matchId, team)
          conversion = 0
		  
      elif "Try" in item.text:
          InsertTry(time, conversion, 0, matchId, team)
          conversion = 0
			 
      elif "Kick at Goal" in item.text:
          InsertPenalty(time, matchId, team)
		  
      elif "Drop Goal" in item.text:
          InsertDropGoal(time, matchId, team)
	
      elif "Missed Conversion" in item.text:
          conversion = 0
	
      elif "Conversion" in item.text:
          conversion = 1
		  
      elif " Card" in item.text:
          if "Yellow" in item.text:
              colour = "Yellow"
          elif "Red" in item.text:
              colour = "Red"
          InsertBooking(time, colour, matchId, team)
    team = team+1

def Connect2DB():
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()
  
  tables=[]
  tables.append("Match")
  tables.append("Try")
  tables.append("Penalty")
  tables.append("DropGoal")
  tables.append("Booking")
  
  for t in tables:
    cursor.execute('SELECT * FROM %s' % t)
    print("\n%s\n" % t)
    for row in cursor:
      print('row = %r' % (row,))

  cursor.close()
  cnxn.close()

def InsertMatchInfo(comp, home, away, dateString, hscore, ascore):
  Month = Enum('Month', 'Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec')
  nums = re.findall('\d+', dateString)
	
  strs = re.findall('[a-zA-Z]+', dateString)
  for s in strs:
    mon = s
  
  date = "%s-%d-%s" % (nums[1], Month[mon].value, nums[0])
  
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()
	
  cursor.execute("SELECT MAX(MatchID) FROM Match")
  for row in cursor:
    id = row[0]+1
  
  cursor.execute("INSERT INTO Match (MatchID, MatchDate, HomeTeam, AwayTeam, Competition, HomeScore, AwayScore) VALUES(?, ?, ?, ?, ?, ?, ?)", (id, date, home, away, comp, hscore, ascore))
  
  cnxn.commit()
  cursor.close()
  cnxn.close()
  
  return id;
  
def InsertTry(time, converted, penalty, matchId, team):
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()

  s = re.findall('\d+', time)
  for a in s:
    t = a
	
  cursor.execute("SELECT MAX(TryID) FROM Try")
  for row in cursor:
    id = row[0]+1
  
  cursor.execute("INSERT INTO Try (TryID, MatchID, TimeInMatch, Team, IsConverted, IsPenaltyTry) VALUES(?, ?, ?, ?, ?, ?)", (id, matchId, t, team, converted, penalty))
  
  cnxn.commit()
  cursor.close()
  cnxn.close()
  
def InsertPenalty(time, matchId, team):
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()

  s = re.findall('\d+', time)
  for a in s:
    t = a
	
  cursor.execute("SELECT MAX(PenaltyID) FROM Penalty")
  for row in cursor:
    id = row[0]+1
   
  cursor.execute("INSERT INTO Penalty (PenaltyID, MatchID, TimeInMatch, Team) VALUES(?, ?, ?, ?)", (id, matchId, t, team))
  
  cnxn.commit()
  cursor.close()
  cnxn.close()

def InsertBooking(time, colour, matchId, team):
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()

  s = re.findall('\d+', time)
  for a in s:
    t = a
	
  cursor.execute("SELECT MAX(BookingID) FROM Booking")
  for row in cursor:
    id = row[0]+1
   
  cursor.execute("INSERT INTO Booking (BookingID, MatchID, TimeInMatch, Team, CardColour) VALUES(?, ?, ?, ?, ?)", (id, matchId, t, team, colour))
  
  cnxn.commit()
  cursor.close()
  cnxn.close()
	  
def InsertDropGoal(time, matchId, team):
  cnxn = pyodbc.connect("Driver={SQL Server Native Client 11.0};Server=LOCALHOST\\SQLEXPRESS;Database=RugbyMatches;Trusted_Connection=yes;")
  cursor = cnxn.cursor()

  s = re.findall('\d+', time)
  for a in s:
    t = a
	
  cursor.execute("SELECT MAX(DropGoalID) FROM DropGoal")
  for row in cursor:
    id = row[0]+1
  
  cursor.execute("INSERT INTO DropGoal (DropGoalID, MatchID, TimeInMatch, Team) VALUES(?, ?, ?, ?)", (id, matchId, t, team))
  
  cnxn.commit()
  cursor.close()
  cnxn.close()
	  
def testfunc(link):
  print(link)
	  
#Define a main() function that prints a litte greeting
def main():
  #Connect2Web()
  Connect2DB()

# This is the standard boilerplate that calls the maun function.
if __name__ == '__main__':
    main()