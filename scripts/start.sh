#!/bin/bash
if [[ $EUID -ne 0 ]]; then
   echo "Este script debe ser ejecutado como root" >&2
   exit 1
fi

# Crear el directorio de notas si no existe
NOTES_DIR="/var/lib/notes/"
if [ ! -d "$NOTES_DIR" ]; then
    mkdir -p "$NOTES_DIR"
    echo "Directorio de notas creado en $NOTES_DIR"
else
    echo "El directorio de notas ya existe en $NOTES_DIR"
fi

# Cambiar los permisos del directorio para que cualquier usuario pueda escribir en él
chown root:root "$NOTES_DIR"
chmod 1777 "$NOTES_DIR"

echo "El sistema de notas ha sido inicializado correctamente."
