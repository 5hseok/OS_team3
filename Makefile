CC = gcc
CFLAGS = -Iheader
LDFLAGS = -lpthread
ODIR = object
SDIRS = source system

SOURCES = $(wildcard $(SDIRS:=/*.c))

OBJECTS = $(patsubst %,$(ODIR)/%,$(notdir $(SOURCES:.c=.o)))

TARGET = LINUXOS

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(ODIR)/%.o: source/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR)/%.o: system/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ODIR):
	mkdir -p $(ODIR)

clean:
	rm -f $(ODIR)/*.o $(TARGET)

all: $(TARGET)

.PHONY: clean all
