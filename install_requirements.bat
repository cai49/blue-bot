@echo "Installing Requirements"

call cd ..
call python -m venv .\blue-bot\
call cd .\blue-bot\
call .\Scripts\activate
call pip install -r .\requirements.txt
