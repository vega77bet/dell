# Validação e critérios de release

Os resultados deste build devem ser consultados em `out/`: `compile.log`, `boot-report.json`, `boot-bios.log`, `boot-uefi.log`, `iso-structure.txt` e o checksum. Uma lista de critérios não implica que todos foram aprovados.

O relatório factual desta entrega está em [BUILD-REPORT.md](BUILD-REPORT.md). Ele registra o SHA da ISO, os boots USB virtuais em BIOS/UEFI, a instalação em disco descartável e as limitações que ainda dependem do Dell físico.

Os testes Qt verificam persistência UTF-8, substituição atômica de texto, undo/redo e exportação PNG, cópia/movimentação recursiva, recusa de sobrescrita, recusa de cópia para dentro da origem e links simbólicos cíclicos. O teste QEMU usa a ISO como armazenamento USB (padrão) ou CD-ROM (`--media cdrom`), firmware BIOS/UEFI, 3 GB de RAM e aceleração por software; só aprova após o probe confirmar o processo do shell **e** a janela real do painel X11. Salva log serial e captura do desktop, vinculando o resultado ao SHA-256 da ISO. Não basta aparecer GRUB ou uma mensagem do kernel.

Antes de instalar em um computador com dados, execute também:

- Instalação descartável em QEMU com disco qcow2 ou raw esparso vazio de pelo menos 20 GB, em BIOS e UEFI; testar instalação guiada e verificar particionamento manual em um disco descartável. `bash tests/install-vm.sh bios` (ou `uefi`) cria apenas `out/install-test.raw` e recusa substituir uma imagem existente. Controle a VM pelo QMP usando `tests/vm-control.py`.
- Remover a ISO da VM e iniciar apenas pelo disco instalado; conferir usuário/senha, desktop e GRUB. Conferir que `live` não existe, `/etc/sudoers.d/dell-live` foi removido e sudo solicita senha do usuário.
- Abrir/salvar texto com acentos, cancelar diálogo de saída sem perder alterações, desenhar e exportar PNG/JPEG, copiar/mover/renomear pasta, restaurar lixeira, testar shell interativo e abas.
- Reproduzir áudio WAV/MP3 e vídeo MP4/WebM; capturar tela e janela; testar ausência de câmera e microfone e usar hardware disponível para foto, gravação e medição de nível.
- Conectar e ejetar USB, navegar na internet, alternar Wi-Fi, ajustar volume e tela, suspender/retomar, desligar e reiniciar.
- Medir memória após 60 segundos de repouso (`free -m`) e verificar respostas com 3 GB no notebook. A meta de RAM não é um resultado medido.

Pendências ou testes que falharem devem permanecer documentados. Uma primeira ISO de desenvolvimento não deve ser chamada de release 1.0 nem de compatibilidade certificada com um notebook ainda não testado.
