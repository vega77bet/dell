# Dell OS v0.1 — relatório de entrega

## Resultado

A ISO abaixo foi gerada e validada como mídia Live inicializável em BIOS Legacy e UEFI de 64 bits:

```text
SHA-256: e4f7b8e56a33d062cef798bb765f83cbdb9966aaf70eb453c31ddb5294797252
Tamanho: aproximadamente 973 MiB
Arquitetura: amd64/x86-64
```

## Verificações executadas

- Testes C++/Qt: aprovados, incluindo texto UTF-8, gravação atômica, histórico do Paint, exportação PNG, operações recursivas de arquivos e proteção contra cópia/movimentação inválida.
- Aplicativos nativos: Text, Paint, Files, Terminal, Media, Capture e Settings iniciaram em uma sessão X11 isolada.
- Boot da ISO como dispositivo USB virtual em BIOS: aprovado; desktop próprio detectado em 377,3 s com 3072 MiB.
- Boot da ISO como dispositivo USB virtual em UEFI: aprovado; desktop próprio detectado em 147,4 s com 3072 MiB.
- Instalação guiada em disco RAW descartável de 20 GB no QEMU: concluída em BIOS.
- Primeiro boot somente pelo disco instalado: concluído; login do usuário criado, GRUB, desktop e terminal funcionaram.
- Pós-instalação: o usuário Live foi removido, o sudo Live foi removido e o sudo passou a exigir senha.
- Memória observada no sistema instalado de teste: 2978 MiB reconhecidos; swap de zram e arquivo de swap disponíveis.

## Limites conhecidos

O teste físico no Dell amarelo do usuário ainda precisa ser feito. Portanto, Wi-Fi, câmera, microfone, áudio, suspensão, teclas especiais, resolução do painel e eventuais particularidades do firmware não estão certificados. A ISO é amd64, requer Secure Boot desativado e foi validada em QEMU; o teste de instalação UEFI completo e o particionamento manual ainda são pendências de hardware/QA.

O editor de vídeo, a loja gráfica, sincronização e snapshots não fazem parte da v0.1. O navegador e os componentes de mídia usam programas e bibliotecas reais do sistema, sujeitos ao limite de 3 GB de RAM.

