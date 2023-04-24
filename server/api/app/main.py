from fastapi import FastAPI
from app.generate import getJSON
from fastapi.middleware.cors import CORSMiddleware

app = FastAPI()

origins = [
    "http://localhost",
    "http://localhost:3000",
    "https://led.ueckoken.club/",
    "http://led.ueckoken.club/"
]

app.add_middleware(
    CORSMiddleware,
    allow_origins=origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

@app.get('/')
def hello():
  return "Alive"

@app.get("/test/{text}")
def returnJSON(text:str):
  return getJSON(text)

data = getJSON("ようこそ工学研究部へ！ Twitter:@ueckoken")

@app.post("/json/{text}")
def setData(text:str):
  global data
  data = getJSON(text)
  return "OK"

@app.get("/json")
def getData():
  global data
  return data