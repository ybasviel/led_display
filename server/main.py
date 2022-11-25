from fastapi import FastAPI
from generate import getJSON

app = FastAPI()

@app.get('/')
def hello():
  return "Alive"

@app.get("/test/{text}")
def returnJSON(text:str):
  return getJSON(text)

data = {}

@app.get("/json/set/{text}")
def setData(text:str):
  global data
  data = getJSON(text)
  return "OK"

@app.get("/json/get")
def getData():
  global data
  return data