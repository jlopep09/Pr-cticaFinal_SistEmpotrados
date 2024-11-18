from fastapi import FastAPI
from routers.router import router as router


app = FastAPI()


app.include_router(router)