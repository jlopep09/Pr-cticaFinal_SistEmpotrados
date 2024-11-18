from fastapi import APIRouter, HTTPException
from fastapi.responses import FileResponse
from fastapi import Response
from  controllers.db_controller import get_reads, add_read
from models.models import SensorData




router = APIRouter(tags = ["api"], prefix="/data")



@router.get("/")
async def root():

    return {"response": "Hellooo"}



# Crear el endpoint POST para recibir datos del sensor
@router.post("/sensordata")
async def receive_data(data: SensorData):
    # Imprimir los datos en la consola
    #print(f"Temperature: {data.temperature} °C, Humidity: {data.humidity} %, Light: {data.light_level} ")
    
    return add_read(data)

@router.get("/reads")
async def receive_data(data: SensorData):

    return get_reads()
