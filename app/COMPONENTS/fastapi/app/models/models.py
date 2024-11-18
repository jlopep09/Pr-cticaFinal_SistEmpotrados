from pydantic import BaseModel
# Definir el modelo de datos para la solicitud
class SensorData(BaseModel):
    temperature: float
    humidity: float
    light_level: int