from fastapi import APIRouter, HTTPException
from fastapi.responses import FileResponse
from fastapi import Response


from pydantic import BaseModel

router = APIRouter(tags = ["api"], prefix="/data")



@router.get("/")
async def root():

    return {"response": "Hellooo"}

# Definir el modelo de datos para la solicitud
class SensorData(BaseModel):
    temperature: float
    humidity: float
    light_level: int

# Crear el endpoint POST para recibir datos del sensor
@router.post("/sensordata")
async def receive_data(data: SensorData):
    # Imprimir los datos en la consola
    print(f"Temperature: {data.temperature} °C, Humidity: {data.humidity} %, Light: {data.light_level} ")
    
    return {"message": "Data received successfully"}
