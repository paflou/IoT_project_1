import subprocess
from dotenv import load_dotenv
import os

# Load environment variables from .env file
load_dotenv()
uri = os.getenv("MONGODB_EXPORT_URI")

collection = "data"  # Name of the collection to export
fields = "ID,Time,Count,Temperature,Humidity"  # Comma-separated list of fields
output_file = "export.csv"

# Build the mongoexport command
command = [
    "mongoexport",
    f"--uri={uri}",
    f"--collection={collection}",
    "--type=csv",
    f"--out={output_file}",
    f"--fields={fields}"
]

# Run the command
try:
    subprocess.run(command, check=True)
    print(f"Exported to {output_file} successfully.")
except subprocess.CalledProcessError as e:
    print("Export failed:", e)
