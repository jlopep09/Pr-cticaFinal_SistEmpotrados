import mariadb
import sys
from fastapi import HTTPException, status
from models.models import SensorData

def get_con():
    try:
        conn = mariadb.connect(
            user="root",
            password="123456",
            host="mariadb_atm",
            port=3306,
            database="esp32db"
        )
        return conn
    except mariadb.Error as e:
        raise HTTPException(status_code=status.HTTP_400_BAD_REQUEST, detail=f"Error connecting to MariaDB Platform: {e}")

def get_reads():
    reads = []
    try:
        conn = get_con()
        cur = conn.cursor()

        # retrieving information 
        cur.execute("SELECT * FROM sensor_reads") 
        columns = [col[0] for col in cur.description]  # Obtener nombres de las columnas
        for row in cur.fetchall():
            read = dict(zip(columns, row))  # Combina nombres de columnas con valores
            reads.append(read)

        conn.close()
        return {"Reads": reads}  # Devuelve como JSON
        
    except mariadb.Error as e:
        print(f"Error retrieving table information: {e}")
        return {"error": "Failed to retrieve reads"}
    
def add_read(data: SensorData):
    try:
        # Obtener conexión
        conn = get_con()
        cur = conn.cursor()

        # Crear la consulta para insertar los datos
        query = """
            INSERT INTO sensor_reads (temperature, humidity, light)
            VALUES (%s, %s, %s)
        """
        # Ejecutar la consulta con los valores
        cur.execute(query, (data.temperature, data.humidity, data.light_level))
        
        # Confirmar los cambios
        conn.commit()

        # Cerrar la conexión
        conn.close()
        return {"message": "Sensor data saved successfully"}
    
    except mariadb.Error as e:
        print(f"Error inserting sensor data: {e}")
        return {"error": "Failed to save sensor data"}