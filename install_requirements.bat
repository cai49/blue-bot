@echo "Installing Requirements"

python -m venv .\BlueBot > $null
.\Scripts\activate
pip install -r requirements.txt > $null
deactivate