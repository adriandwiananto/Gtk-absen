CFLAGS :=-g -Wall -export-dynamic
CXXFLAGS :=-g -Wall -export-dynamic
C_SOURCES := main.c window_switcher.c mainmenu.c registration.c config.c new_nfc.c new_qr.c misc.c json.c network.c sending.c
CXX_SOURCES := picc_emulation_write.cpp
C_EXECUTABLE=absen
CXX_EXECUTABLE=picc_emulation_write

all: $(C_EXECUTABLE) $(CXX_EXECUTABLE)

$(C_EXECUTABLE): $(C_SOURCES)
	$(CC) $(CFLAGS) -o $(C_EXECUTABLE) $(C_SOURCES) `pkg-config gtk+-3.0 libglade-2.0 --cflags --libs` `pkg-config --cflags --libs libconfig` -lssl -lcrypto -lsqlite3 -ljson -lcurl -lhpdf

$(CXX_EXECUTABLE): $(CXX_SOURCES)
	$(CXX) $(CXX_FLAGS) -o $(CXX_EXECUTABLE) $(CXX_SOURCES) `pkg-config --cflags --libs libconfig` -lssl -lcrypto -lCVAPIV01_DESFire
	
clean:
	rm -rf *o  $(EXECUTABLE)
