# Nom du compilateur
CC = gcc

# Options de compilation
CFLAGS = -Wall -O2 -fno-tree-vectorize

# Fichiers sources
SRC_NON_VECTOR = lab06.c
SRC_VECTOR = lab06_vec.c

# Noms des exécutables
EXE_NON_VECTOR = lab06
EXE_VECTOR = lab06_vec

# Cible par défaut
all: $(EXE_NON_VECTOR) $(EXE_VECTOR)

# Compilation de la version non vectorisée
$(EXE_NON_VECTOR): $(SRC_NON_VECTOR)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation de la version vectorisée
$(EXE_VECTOR): $(SRC_VECTOR)
	$(CC) $(CFLAGS) -mavx -o $@ $^

# Cible pour nettoyer les fichiers objets et les exécutables
clean:
	rm -f $(EXE_NON_VECTOR) $(EXE_VECTOR) *.o

.PHONY: all clean
