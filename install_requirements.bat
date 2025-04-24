@echo "Installing Requirements"

call git clone https://github.com/cai49/blue-bot.git
call python -m venv .\blue-bot\
call cd .\blue-bot\
call .\Scripts\activate
call pip install -r .\requirements.txt
