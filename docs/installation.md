# Pendrive, boot e instalação

1. Faça backup dos arquivos do notebook e do pendrive. Confirme que o processador suporta 64 bits (no Linux, `lscpu`, linha CPU op-mode(s); no Windows, Informações do Sistema).
2. Confira `sha256sum -c dell-os-v0.1-amd64.iso.sha256` na pasta da ISO. No Windows, `Get-FileHash .\dell-os-v0.1-amd64.iso -Algorithm SHA256` deve coincidir com o arquivo de checksum.
3. Grave a ISO com Rufus (modo DD), Etcher ou GNOME Discos. Confira modelo, capacidade e identificação do **pendrive** na ferramenta antes de confirmar. Todos os dados dele serão apagados.
4. No Dell, abra o menu de boot (normalmente F12 ao ligar). Selecione o USB em BIOS/Legacy ou UEFI. Desative Secure Boot para esta versão. Não troque o modo SATA sem necessidade.
5. Escolha Experimentar ou instalar. Se a tela não aparecer, reinicie pelo USB e tente Vídeo seguro. A primeira inicialização pode demorar em um pendrive lento.
6. Na sessão Live, teste teclado, touchpad, Wi-Fi, áudio, USB e webcam. Use `live` / `live` se um login for solicitado. A sessão Live não preserva arquivos ao desligar; salve em outro disco se necessário.
7. Abra Instalar Dell OS. Selecione idioma, região, teclado, disco, usuário e senha. O modo Apagar disco elimina **todos** os arquivos do disco escolhido. O modo manual permite configurar as partições. Nesta versão não há promessa de dual boot automático: mantenha backups e use o modo manual somente com conhecimento de particionamento.
8. BIOS: use MBR ou configure uma partição BIOS boot se optar manualmente por GPT. UEFI: inclua uma partição FAT32 EFI de pelo menos 300 MiB, montada em `/boot/efi`; recomenda-se 512 MiB. Use ext4 para `/`. A instalação guiada cuida dessa preparação.
9. A configuração padrão propõe swap em arquivo. Não configure hibernação; a v0.1 usa suspensão para RAM e zram. É possível escolher swap particionada ou nenhuma swap em disco.
10. Revise o resumo e confirme. Espere finalizar e desligue. Remova o USB e ligue novamente. Confirme o login no sistema instalado, sem o pendrive.

Se a instalação falhar, mantenha a sessão aberta e preserve o log em `~/.cache/calamares/session.log` (ou `/root/.cache/calamares/session.log` quando executado por pkexec). Não tente apagar outras partições para “corrigir” um erro sem entender a causa.

O script de build nunca executa `dd`, `mkfs` ou `parted` contra o computador de desenvolvimento. As únicas operações reais de particionamento são iniciadas pelo Calamares, no disco escolhido explicitamente pelo usuário.
