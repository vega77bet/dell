# Dell OS 0.1 — Aurora

Distribuição pessoal Linux amd64 com desktop e aplicativos Qt 6/C++, base Debian 13, sessão X11/Openbox e instalador Calamares. Inicia em modo Live e instala no disco por BIOS/Legacy ou UEFI (Secure Boot desativado).

## Construir e testar

Use uma máquina Linux **de desenvolvimento** com Docker, internet, pelo menos 20 GB livres (40 GB com Docker VFS), 4 GB de RAM e CPU amd64. O Dell de 3 GB é o destino do sistema; não precisa compilar nele. As imagens de desenvolvimento e dependências são muito maiores que o sistema em execução.

```sh
bash build/sdk.sh   # compila C++, executa os testes e prepara os binários
bash build/iso.sh   # gera out/dell-os-v0.1-amd64.iso e checksum
bash build/smoke.sh # inicia a ISO real em QEMU, BIOS e UEFI, com 3 GB
```

Os scripts não gravam em dispositivos físicos. `out/` contém logs, binários, manifesto de pacotes na imagem, relatório JSON de boot e capturas PPM dos boots bem-sucedidos. O build de pacotes usa a distribuição fixa `trixie`, recebendo suas atualizações de segurança; isso permite reconstrução do processo, **não** garante ISO idêntica byte a byte. Registre o SHA do código, o manifesto e os digests das imagens ao arquivar uma versão.

Se as imagens `dell-os-system` e `dell-os-iso-tools` já foram construídas com o código atual, `bash build/iso.sh --reuse-images` repete apenas o empacotamento. Não use essa opção depois de mudar o sistema ou os aplicativos sem reconstruir as imagens correspondentes.

Para desenvolver apenas os aplicativos em Debian 13, instale `build-essential cmake ninja-build qt6-base-dev qt6-multimedia-dev qt6-svg-dev libqtermwidget-dev libutf8proc-dev libx11-dev` e use CMake. O teste de widgets roda offscreen. Para o desktop use uma sessão X11 isolada; executar `dell-shell` na sua sessão principal pode cobrir seu desktop.

## Gravar no pendrive

Confira o checksum antes. No Windows, use **Rufus**, selecione a ISO e, se perguntado, escolha modo **DD**. Balena Etcher também aceita a ISO. Um pendrive de pelo menos 4 GB é recomendado; a gravação apaga seu conteúdo. Não é necessário extrair a ISO.

No Linux, prefira a interface do Etcher ou GNOME Discos. Consulte [o guia de boot e instalação](docs/installation.md) para o procedimento, identificação do disco e restrições. Nenhum disco é apagado apenas ao iniciar a ISO. O instalador exige escolher o destino e confirmar o resumo.

## O que está incluído

Desktop próprio com três wallpapers vetoriais gerados localmente, menu com busca, tarefas, relógio, temas, notificações via Dunst, bandeja, Wi-Fi, montagem de USB e gerenciamento de energia. Aplicativos `dell-app files|text|paint|terminal|media|capture|settings|welcome` são processos nativos, com funções de arquivos, editor UTF-8, desenho/exportação/histórico, terminal PTY com abas, reprodução/lista de mídia, captura X11, gravação/meter de áudio e câmera/fotos. O navegador é Firefox ESR; os componentes de rede, áudio, usuários e hardware abrem ferramentas Linux reais nas Configurações.

O instalador Calamares usa o squashfs do pendrive, cria partições e swap, configura usuário, senha, idioma, teclado, fuso e GRUB. Na instalação remove o usuário Live, seu sudo sem senha e os pacotes de boot Live. Não depende da internet para copiar o sistema.

## Compatibilidade e limitações

- Só x86-64/amd64. Processadores exclusivamente 32 bits não são compatíveis. Confirme modelo e arquitetura do Dell antes da instalação.
- A interface principal é em português; alguns diálogos dos componentes de terceiros permanecem em inglês nesta versão.
- UEFI de 64 bits e BIOS Legacy. UEFI de 32 bits e Secure Boot não foram implementados.
- A sessão Live usa `live` / `live`, sem persistência. O usuário instalado define sua própria senha e usa login normal.
- Firmware Intel/Realtek/Atheros está incluído, mas a compatibilidade depende do dispositivo. Alguns Wi-Fi Broadcom exigem firmware/driver adicional.
- Meta de desktop em repouso: até 700 MiB, a medir no hardware real. Não é uma medição nem garantia de desempenho. Firefox e vídeo são os maiores consumidores. Use poucas abas e vídeos modestos; não existe aceleração garantida.
- Editor de vídeo, loja gráfica, sincronização e recuperação por snapshots ficam fora da v0.1, conforme o plano aprovado. Atualização real via APT, recuperação básica pelo Live e opções do GRUB.
- Webcam e áudio só funcionam com dispositivos e drivers disponíveis; ausência é apresentada ao usuário. Os testes automatizados não certificam hardware físico.
- Paint: até 2048×2048 ao criar/redimensionar e 4096×4096 ao abrir, 12 ações de histórico. Texto: até 16 MB, UTF-8. Mover/copiar não sobrescreve destinos existentes.

Veja [arquitetura](docs/architecture.md), [validação](docs/validation.md) e o [relatório factual desta entrega](docs/BUILD-REPORT.md). Projeto pessoal sem afiliação com a Dell Inc. Preserve os avisos de copyright e licenças dos componentes redistribuídos; `/usr/share/doc/*/copyright` contém seus termos.
