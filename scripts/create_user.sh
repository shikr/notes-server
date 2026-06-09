#!/bin/bash
set -euo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

info() {
    echo -e "${CYAN}[INFO]${NC}  $*"
}
ok() {
    echo -e "${GREEN}[OK]${NC}    $*"
}
warn() {
    echo -e "${YELLOW}[WARN]${NC}  $*"
}
error() {
    echo -e "${RED}[ERROR]${NC} $*"
}

if [ "$EUID" -ne 0 ]; then
    error "Este script debe ejecutarse como root."
    exit 1
fi

if [ "$#" -lt 2 ]; then
    error "Uso: $0 <usuario> <contraseña>"
    exit 1
fi

USER="$1"
PASSWORD="$2"
SSHD_CONFIG="/etc/ssh/sshd_config"
BACKUP="$SSHD_CONFIG.bak.$(date +%Y%m%d%H%M%S)"
COMMAND="/usr/local/bin/notes-client"

if [[ ! -x "$COMMAND" ]]; then
    error "El comando '$COMMAND' no existe o no es ejecutable."
    exit 1
fi

if id "$USER" &>/dev/null; then
    warn "El usuario '$USER' ya existe. Se omitirá la creación."
else
    info "Creando el usuario '$USER'..."
    useradd -m -s /bin/bash "$USER"
    echo "$USER:$PASSWORD" | chpasswd
    ok "Usuario '$USER' creado."
fi

info "Creando respaldo de '$SSHD_CONFIG' en '$BACKUP'..."
cp "$SSHD_CONFIG" "$BACKUP"
ok "Respaldo creado."

if grep -q "^Match User $USER$" "$SSHD_CONFIG"; then
    warn "Se encontró una sección de configuración previa del usuario '$USER' en '$SSHD_CONFIG'. Reemplazando..."
    # Eliminar la sección existente
    perl -i -0777 -pe "s/\n?^Match User ${USUARIO}\n(\s+.*\n)*//m" "$SSHD_CONFIG"
fi

info "Agregando configuración SSH para el usuario '$USER'..."
cat >> "$SSHD_CONFIG" <<EOF

# --- Bloque generado por create_user.sh para el usuario '$USER' ---
Match User $USER
    ForceCommand $COMMAND
    AllowTcpForwarding no
    X11Forwarding no
    PermitTTY yes
# -----------------------------------------------------------------
EOF

ok "Configuración SSH agregada para el usuario '$USER'."

info "Validando la configuración SSH..."
if sshd -t; then
    ok "La configuración SSH es válida."
else
    error "La configuración SSH tiene errores. Restaurando el respaldo..."
    cp "$BACKUP" "$SSHD_CONFIG"
    error "Configuración restaurada. Por favor, revise el archivo '$SSHD_CONFIG' para corregir los errores."
    exit 1
fi

info "Reiniciando el servicio SSH..."
if systemctl reload ssh 2>/dev/null; then
    ok "Servicio SSH recargado correctamente."
else
    error "No se pudo recargar el servicio SSH. Recárgalo manualmente:"
    error "  systemctl reload ssh"
    exit 1
fi

echo
echo -e "${GREEN}============================================================${NC}"
echo -e "${GREEN}  Configuración completada${NC}"
echo -e "${GREEN}============================================================${NC}"
echo -e "  Usuario:       ${CYAN}${USER}${NC}"
echo -e "  ForceCommand:  ${CYAN}${COMMAND}${NC}"
echo -e "  Respaldo:      ${CYAN}${BACKUP}${NC}"
echo -e "${GREEN}============================================================${NC}"
